#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <alsa/seq.h>
#include <alsa/seq_event.h>
#include <alsa/seqmid.h>
#include <alsa/seq_midi_event.h>
#include <stdio.h>
#include "bperform.h"
#define PORTACCNUM 84

void targetMidiPortSelected( GtkWidget *menuItem);
void ins0typeSelected( GtkWidget* combo, effects* ins0p);
void ins1typeSelected( GtkWidget* combo, effects* ins1p);
void ins0targetChnlSelected( GtkWidget* combo);
void ins1targetChnlSelected( GtkWidget* combo);
void ins0edit(GtkWidget* button, insStrip* ins0stripp);

snd_seq_t *handle;
int source; // source alsa-client id;
int sport = 0; // app's source MIDI port number
int tport = 0; // target client's midi port number
int portaStat = 0; // portament off(0-63) / on(64-127)

struct _midiTarget midiTargets[10];
int dstMaxEntries = 0;

void portaCheckBoxChecked(void)
{
	if(portaStat){ // as the result of checking behavior, portament enabled
		sendCc(PORTACCNUM, 127);
	}else{
		sendCc(PORTACCNUM, 0);
	}

	portaStat = !portaStat;
}

void portaTimeChanged(GtkWidget* scale)
{
	guint val = gtk_range_get_value( GTK_RANGE(scale) );
	//g_print("%d\n", val);
	sendCc(5, val);
}

void programSelected(GtkWidget* pListComboBox, GList* toneEntries){
	gchar* pName;
	GList* list;

	list = toneEntries;

	pName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(pListComboBox) );

	do {
		if( !strcmp(pName, ((toneEntry*)(list->data))->name ) ) break;
		list = list->next;
	} while( list );

	sendCc(0, atoi( ((toneEntry*)(list->data))->msb) ); // bank select MSB 
	sendCc(32, atoi( ((toneEntry*)(list->data))->lsb) ); // bank select LSB
	pgmChange( atoi( ((toneEntry*)(list->data))->pc) );

}

static void createInsTypeComboBox(GtkWidget* comboBox, effects* insp)
{
	GList* list = insp->effectList;

	do {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox),\
			((insEffect*)(list->data))->name);
		list = list->next;
	} while( list );

}

static void createInsTargetChnlComboBox(GtkWidget* comboBox)
{
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "Off");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "1");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "2");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "3");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "4");
}

static void createVarTargetComboBox(GtkWidget* comboBox)
{
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "Off");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "1");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "2");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "3");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "4");
}

void createProgramListComboBox(GtkWidget* comboBox, tones* tonesp)
{
	FILE *fp;
	char* pn = NULL;
	char* msb = NULL;
	char* lsb = NULL;
	char* pc = NULL;
	size_t n = 0;
	toneEntry* entry;
	GList* list = NULL;

	if( (fp = fopen("./entries.txt", "r")) ){
		while( ( getdelim(&pn, &n, '\t', fp) ) != -1 ){
			getdelim(&msb, &n, '\t', fp);
			getdelim(&lsb, &n, '\t', fp);
			getdelim(&pc, &n, '\n', fp);
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), pn);

			entry = (toneEntry*)malloc(sizeof(toneEntry));
			strcpy(entry->name, pn);
			strcpy(entry->msb, msb);
			strcpy(entry->lsb, lsb);
			strcpy(entry->pc, pc);

			list = g_list_append(list, entry);
		}
		fclose(fp);
	}
	tonesp->toneEntries = list;
}

gboolean delete_event (void)
{
	g_print("delete event occurred\n");
    snd_seq_disconnect_to(handle, sport, source, tport); 

		// If returingn TRUE, application itself continues running.
	return FALSE; // By FALSE, destroy handler will be called.
}

void destroy(void)
{
	g_print("destroy handler\n");
	gtk_main_quit();
}

static gboolean quit_button_pushed
(GtkWidget* widget, GdkEvent *event, gpointer data)
{
    snd_seq_disconnect_to(handle, sport, source, tport); 
	gtk_main_quit();
	return TRUE;
}

gboolean closeEditWindow(GtkWidget *widget, insStrip* insStripp)
{
	//g_print("effect type: %s subwindow closed.\n", insStripp->effectInfo->currentInsType);
	//gtk_widget_destroy(widget);
	g_print("edit window closed\n");
	g_print("edit window closed\n");
	g_print("edit window closed\n");
	g_print("edit window closed\n");
	return FALSE;
}

void reverbsend( GtkRange *range, GdkEvent *event, gpointer data)
{
	static guint cc = 91;
	guint val = (guint) gtk_range_get_value(range);
	sendCc(cc, val); // reverb send
	//g_print("%d\n", val);
}

int main(int argc, char** argv)
{
	int itr;
	effects ins0;
	effects ins1;
	insStrip ins0strip;
	insStrip ins1strip;
	tones tones;

		// variables for alsa connection destination port
	snd_seq_client_info_t *cinfo;
	snd_seq_port_info_t *pinfo;

		// preparation of dst port searching
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_set_client(cinfo, -1);
	char midiTargetName[128];

		// main window instances
	GtkWidget* window;
	GtkWidget* base; // acommodate menubar on top and others on bottom
	GtkWidget* menubar; // occupies top place
	GtkWidget* exceptMenu; // occupies button place

		// edit window instances
	GtkWidget* editWindow;

	GtkWidget* voicePages;

	// page0 entries
	GtkWidget* voicePage0;
	GtkWidget* page0contents;
	GtkWidget* page0left;
	GtkWidget* page0right;
	GtkWidget* portaCheckBox;
	GtkWidget* portaTimeScale;
	GtkWidget* volBox;
	GtkWidget* volLabel;
	GtkWidget* volScale;
	GtkWidget* panBox;
	GtkWidget* panLabel;
	GtkWidget* panScale;
	GtkWidget* attackBox;
	GtkWidget* attackLabel;
	GtkWidget* attackScale;
	GtkWidget* decayBox;
	GtkWidget* decayLabel;
	GtkWidget* decayScale;
	GtkWidget* releaseBox;
	GtkWidget* releaseLabel;
	GtkWidget* releaseScale;
	GtkWidget* revSendBox;
	GtkWidget* revSendScale;
	GtkWidget* revSendLabel;
	GtkWidget* choSendBox;
	GtkWidget* choSendScale;
	GtkWidget* choSendLabel;

	// page1 entries
	GtkWidget* voicePage1;
	GtkWidget* page1contents;

	GtkWidget* pListComboBox;

	// system effect entries
	GtkWidget* variationBox;
	GtkWidget* varTarget;
	GtkWidget* varScale;
	GtkWidget* varLabel;
	GtkWidget* varEdit;

	GtkWidget* insert0box;
	GtkWidget* ins0type;
	GtkWidget* ins0scale;
	GtkWidget* ins0targetChnl;
	GtkWidget* ins0label;
	GtkWidget* ins0editButton;

	GtkWidget* insert1box;
	GtkWidget* ins1type;
	GtkWidget* ins1scale;
	GtkWidget* ins1targetChnl;
	GtkWidget* ins1label;
	GtkWidget* ins1editButton;

	GtkWidget* choReturnBox;
	GtkWidget* choRetScale;
	GtkWidget* choRetLabel;
	GtkWidget* choEditButton;
	GtkWidget* choBlankBox;

	GtkWidget* revReturnBox;
	GtkWidget* revReturnScale;
	GtkWidget* revReturnLabel;
	GtkWidget* revEditButton;
	GtkWidget* revBlankBox;

	GtkWidget* help;
	GtkWidget* helpDownlist;
	GtkWidget* quit;
	GtkWidget* connect;
	GtkWidget* connectDownlist;
	GtkWidget* init;
	GtkWidget* initDownlist;
	GtkWidget* initialize;
	GtkWidget* midiDstEachEntry; // variable for generating downlist entries in loop

	source = snd_seq_open( &handle, "default", SND_SEQ_OPEN_DUPLEX, 0 );
	if( source < 0 ){
		perror("seq handler open failed");
		return 0;
	}
	snd_seq_set_client_name(handle, "bperform");

	sport = snd_seq_create_simple_port(
		handle, "bperform-port",
		SND_SEQ_PORT_CAP_READ| SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_SOFTWARE
	);

	if ( sport < 0){
		perror("port open failed");
		return 0;
	}

	gtk_init(&argc, &argv);

		// clear insert effect entries : I don't know why I need it.
	memset(&ins0, 0, sizeof(effects));
	memset(&ins1, 0, sizeof(effects));

		// main window and layouts
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	base = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

		// edit window and layouts
	editWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	// create menubar bar
	menubar = gtk_menu_bar_new();

	initDownlist = gtk_menu_new();
	init = gtk_menu_item_new_with_label("Initialize");
	initialize = gtk_menu_item_new_with_label("Initialize MU100B");

	connectDownlist = gtk_menu_new();
	connect = gtk_menu_item_new_with_label("Connect");

	helpDownlist = gtk_menu_new();
	help = gtk_menu_item_new_with_label("Help");
	quit = gtk_menu_item_new_with_label("Quit");

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), init);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(init), initDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), initialize);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), connect);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(connect), connectDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpDownlist), quit);

	// midi connection target listing up
    while (snd_seq_query_next_client(handle, cinfo) >= 0) {
        char strTmp[64];

        /* reset query info */
        snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
        snd_seq_port_info_set_port(pinfo, -1);

		while (snd_seq_query_next_port(handle, pinfo) >= 0) {
			memset(midiTargetName, 0, 128);
			memset(strTmp, 0, 64);
			sprintf(strTmp, "%d", snd_seq_client_info_get_client(cinfo));
			midiTargets[dstMaxEntries].clientId = snd_seq_client_info_get_client(cinfo);
			   // omit client#0, which used in the system.
			if(!midiTargets[dstMaxEntries].clientId) continue;

			strcat(midiTargetName, strTmp);
			strcat(midiTargetName, " / ");
			memset(strTmp, 0, 64);
			sprintf(strTmp, "%d", snd_seq_port_info_get_port(pinfo));
			midiTargets[dstMaxEntries].portId = snd_seq_port_info_get_port(pinfo);
			midiTargets[dstMaxEntries].checked = 0;

			strcat(midiTargetName, strTmp);
			strcat(midiTargetName, "\t");
			memset(strTmp, 0, 64);
			strcat(midiTargetName, snd_seq_port_info_get_name(pinfo));
			strcpy(midiTargets[dstMaxEntries].clientName, midiTargetName);

			midiDstEachEntry = gtk_check_menu_item_new_with_label(midiTargetName);
			//midiDstEachEntry = gtk_menu_item_new_with_label(midiTargetName);
			//gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(midiDstEachEntry), TRUE);
			gtk_menu_shell_append(GTK_MENU_SHELL(connectDownlist), midiDstEachEntry) ;

			midiTargets[dstMaxEntries].instance = midiDstEachEntry;
			dstMaxEntries++;
		}
	}

	
	// create area other than menubar bar
	exceptMenu = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	voicePage0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	voicePage1 = gtk_button_new_with_label("hoge");

	voicePages = gtk_notebook_new();
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voicePage0, GTK_WIDGET(gtk_label_new("Voice0") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voicePage1, GTK_WIDGET(gtk_label_new("Voice1") ) );

/*
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voiceBox, GTK_WIDGET(gtk_label_new("Voice2") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voiceBox, GTK_WIDGET(gtk_label_new("Voice3") ) );
*/

	portaCheckBox = gtk_check_button_new_with_label("Portament");
	portaTimeScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	pListComboBox = gtk_combo_box_text_new();
	volBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	volLabel = gtk_label_new("V");
	volScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	panBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	panLabel = gtk_label_new("P");
	panScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -63, 63, 1);
	attackBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	attackLabel = gtk_label_new("A");
	attackScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	decayBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	decayLabel = gtk_label_new("D");
	decayScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	releaseBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	releaseLabel = gtk_label_new("R");
	releaseScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	page0contents = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	page0left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	page0right = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	createProgramListComboBox( pListComboBox, &tones);

	varTarget = gtk_combo_box_text_new();
	createVarTargetComboBox(varTarget);

	revSendBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	revSendLabel = gtk_label_new("Reverb");
	revSendScale= gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);

	choSendBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	choSendLabel = gtk_label_new("Chorus");
	choSendScale= gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);

	variationBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	varLabel = gtk_label_new("Variation");
	varEdit = gtk_button_new_with_label("Edit");
	varScale= gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);

	choReturnBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); 
	choRetLabel = gtk_label_new("Chorus");
	choRetScale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
	choEditButton = gtk_button_new_with_label("Edit");
	choBlankBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(choBlankBox, -1, 72); // width, height

	revReturnBox =  gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	revReturnLabel = gtk_label_new("Reverb");
	revReturnScale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
	revEditButton = gtk_button_new_with_label("Edit");
	revBlankBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(revBlankBox, -1, 72); // width, height

	insert0box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	ins0label = gtk_label_new("Insert1");
	ins0editButton = gtk_button_new_with_label("Edit");
	ins0scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);
		// create combobox entries with insertion effect type initialization
	ins0targetChnl = gtk_combo_box_text_new();
	ins0type = gtk_combo_box_text_new();
	ins0.range = ins0scale;
	prepIns(&ins0);
		// create combobox entries of effect target channel
	createInsTargetChnlComboBox(ins0targetChnl);
	createInsTypeComboBox(ins0type, &ins0);
		// ins0 object preparation
	ins0strip.insertBox = insert0box;
	ins0strip.insLabel = ins0label;
	ins0strip.insEditButton = ins0editButton;
	ins0strip.insScale = ins0scale;
	ins0strip.insTargetChnl = ins0targetChnl;
	ins0strip.effectInfo = &ins0;
	ins0strip.editWindow = editWindow;

	insert1box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	ins1label = gtk_label_new("Insert2");
	ins1editButton = gtk_button_new_with_label("Edit");
	ins1scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);

	ins1targetChnl = gtk_combo_box_text_new();
	ins1type = gtk_combo_box_text_new();
	ins1.range = ins1scale;
	prepIns(&ins1);

	createInsTargetChnlComboBox(ins1targetChnl);
	createInsTypeComboBox(ins1type, &ins1);

	gtk_scale_set_value_pos(GTK_SCALE(choSendScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(choSendScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(revSendScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(revSendScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(varScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(varScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(ins0scale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(ins0scale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(ins1scale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(ins1scale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(choRetScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(choRetScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(revReturnScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(revReturnScale), TRUE);

	// callback function settings
	g_signal_connect(G_OBJECT(window), "delete_event",\
		G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",\
		G_CALLBACK(destroy), NULL);
	g_signal_connect(G_OBJECT(quit), "activate",\
		G_CALLBACK(quit_button_pushed), NULL);
	g_signal_connect(revSendScale, "value-changed",\
		G_CALLBACK(revSend), NULL);
	g_signal_connect(choSendScale, "value-changed",\
		G_CALLBACK(choSend), NULL);

		// combobox program select
	g_signal_connect(G_OBJECT(pListComboBox), "changed", \
		G_CALLBACK(programSelected), tones.toneEntries);
	g_signal_connect(G_OBJECT(portaTimeScale), "value-changed", \
		G_CALLBACK(portaTimeChanged), NULL);
	g_signal_connect(G_OBJECT(attackScale), "value-changed", \
		G_CALLBACK(attackChanged), NULL);
	g_signal_connect(G_OBJECT(decayScale), "value-changed", \
		G_CALLBACK(decayChanged), NULL);
	g_signal_connect(G_OBJECT(releaseScale), "value-changed", \
		G_CALLBACK(releaseChanged), NULL);

	g_signal_connect(G_OBJECT(ins0type), "changed", \
		G_CALLBACK(ins0typeSelected), &ins0);
	g_signal_connect(G_OBJECT(ins0scale), "value-changed", \
		G_CALLBACK(ins0changed), &ins0);
	g_signal_connect(G_OBJECT(ins0targetChnl), "changed", \
		G_CALLBACK(ins0targetChnlSelected), NULL);
	g_signal_connect(G_OBJECT(ins0editButton), "clicked", \
		G_CALLBACK(ins0edit), &ins0strip);

	g_signal_connect(G_OBJECT(ins1type), "changed", \
		G_CALLBACK(ins1typeSelected), &ins1);
	g_signal_connect(G_OBJECT(ins1scale), "value-changed", \
		G_CALLBACK(ins1changed), &ins1);
	g_signal_connect(G_OBJECT(ins1targetChnl), "changed", \
		G_CALLBACK(ins1targetChnlSelected), NULL);

		// midi connection button actions
	for(itr = 0; itr < dstMaxEntries; itr++){
		g_signal_connect(midiTargets[itr].instance, "activate", \
			G_CALLBACK(targetMidiPortSelected), NULL);
	}


	g_signal_connect(G_OBJECT(portaCheckBox), "clicked", G_CALLBACK(portaCheckBoxChecked), NULL);

	// widgets boxing

	gtk_box_pack_start( GTK_BOX(page0left), pListComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), volBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), panBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), attackBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), decayBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), releaseBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), portaCheckBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), portaTimeScale, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(volBox), volLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(volBox), volScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(panBox), panLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(panBox), panScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(attackBox), attackLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(attackBox), attackScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(decayBox), decayLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(decayBox), decayScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(releaseBox), releaseLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(releaseBox), releaseScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0right), choSendBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0right), revSendBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0contents), page0left, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0contents), page0right, FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(voicePage0), page0contents);

	gtk_box_pack_start( GTK_BOX(choSendBox), choSendLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choSendBox), choSendScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revSendBox), revSendLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revSendBox), revSendScale, TRUE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(variationBox), varLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(variationBox), varTarget, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(variationBox), varScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(variationBox), varEdit, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(insert0box), ins0label, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert0box), ins0targetChnl, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert0box), ins0type, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert0box), ins0scale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert0box), ins0editButton, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(insert1box), ins1label, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert1box), ins1targetChnl, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert1box), ins1type, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert1box), ins1scale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(insert1box), ins1editButton, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(choReturnBox), choRetLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choReturnBox), choBlankBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choReturnBox), choRetScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choReturnBox), choEditButton, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(revReturnBox), revReturnLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revReturnBox), revBlankBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revReturnBox), revReturnScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revReturnBox), revEditButton, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(exceptMenu), voicePages, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), insert0box, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), insert1box, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), variationBox, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), choReturnBox, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), revReturnBox, TRUE, 0, 0);

	gtk_box_pack_start( GTK_BOX(base), menubar, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(base), exceptMenu, TRUE, TRUE, 0);
	gtk_container_add( GTK_CONTAINER(window), base);


	gtk_widget_show_all(window);
	//gtk_range_set_range(GTK_RANGE(revReturnScale), 23, 45);
	gtk_main();
	return 0;
}

void ins0targetChnlSelected( GtkWidget* combo)
{
	gchar *chnl;
	chnl = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	if (!strcmp(chnl, "Off" ) ){
		sendExc(4 , 0x03, 0x00, 0x0c, 0x7f);
	} else if (!strcmp(chnl, "1" ) ){
		sendExc(4 , 0x03, 0x00, 0x0c, 0x0);
	} else if (!strcmp(chnl, "2" ) ){
		sendExc(4 , 0x03, 0x00, 0x0c, 0x1);
	} else if (!strcmp(chnl, "3" ) ){
		sendExc(4 , 0x03, 0x00, 0x0c, 0x2);
	} else if (!strcmp(chnl, "4" ) ){
		sendExc(4 , 0x03, 0x00, 0x0c, 0x3);
	}
}

void ins1targetChnlSelected( GtkWidget* combo)
{
	gchar *chnl;
	chnl = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	if (!strcmp(chnl, "Off" ) ){
		sendExc(4 , 0x03, 0x01, 0x0c, 0x7f);
	} else if (!strcmp(chnl, "1" ) ){
		sendExc(4 , 0x03, 0x01, 0x0c, 0x0);
	} else if (!strcmp(chnl, "2" ) ){
		sendExc(4 , 0x03, 0x01, 0x0c, 0x1);
	} else if (!strcmp(chnl, "3" ) ){
		sendExc(4 , 0x03, 0x01, 0x0c, 0x2);
	} else if (!strcmp(chnl, "4" ) ){
		sendExc(4 , 0x03, 0x00, 0x0c, 0x3);
	}
}

void ins0typeSelected( GtkWidget* combo, effects* ins0p)
{
	gchar *eName; // selected effect name from the combo box entries.
	GList* list = ins0p->effectList;

	eName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	do {
		if( !strcmp(eName, ((insEffect*)(list->data))->name ) ) break;
		list = list->next;
	} while( list );

	strcpy(ins0p->currentInsType, ((insEffect*)list->data)->name);
	sendExc(5 , 0x03, 0x00, 0x00, ((insEffect*)list->data)->msb, ((insEffect*)list->data)->lsb);
	gtk_range_set_range(GTK_RANGE(ins0p->range), ((insEffect*)list->data)->param10.rangeMin,\
		((insEffect*)list->data)->param10.rangeMax);
}

void ins1typeSelected( GtkWidget* combo, effects* ins1p)
{
	gchar *eName; // selected effect name from the combo box entries.
	GList* list = ins1p->effectList;

	eName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	do {
		if( !strcmp(eName, ((insEffect*)(list->data))->name ) ) break;
		list = list->next;
	} while( list );

	strcpy(ins1p->currentInsType, ((insEffect*)list->data)->name);
	sendExc(5 , 0x03, 0x01, 0x00, ((insEffect*)list->data)->msb, ((insEffect*)list->data)->lsb);
	gtk_range_set_range(GTK_RANGE(ins1p->range), ((insEffect*)list->data)->param10.rangeMin,\
		((insEffect*)list->data)->param10.rangeMax);
}


void targetMidiPortSelected( GtkWidget *label){
	const char *menuString;
	char clientNumInStr[4];;
	guint clientNum;
	int itr = 0;

	menuString = gtk_menu_item_get_label(GTK_MENU_ITEM(label));

	memset(clientNumInStr, 0, 4);

	while(menuString[itr] != '/'){
		clientNumInStr[itr] = menuString[itr];
		itr++;
	}

	clientNum = atoi(clientNumInStr);

	for(itr=0; itr< dstMaxEntries ; itr++){
		if( clientNum == midiTargets[itr].clientId) break;
	}

	if( midiTargets[itr].checked ){
		snd_seq_disconnect_to(handle, 0, clientNum, 0);
		midiTargets[itr].checked = 0;
	} else {
		snd_seq_connect_to(handle, 0, clientNum, 0);
		midiTargets[itr].checked = 1;
	}
}

void ins0edit(GtkWidget* button, insStrip* ins0stripp)
{
	GtkWidget* editWindowBox;
	GtkWidget* label;

	label = gtk_label_new(ins0stripp->effectInfo->currentInsType);
	editWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	
	gtk_box_pack_start(GTK_BOX(editWindowBox), label, TRUE, TRUE, 0);
	gtk_container_add( GTK_CONTAINER(ins0stripp->editWindow), editWindowBox);

	g_signal_connect(G_OBJECT(ins0stripp->editWindow), "delete_event",\
		G_CALLBACK(closeEditWindow), ins0stripp);

	gtk_window_set_default_size(GTK_WINDOW(ins0stripp->editWindow), 200, 400);
	gtk_widget_show_all(ins0stripp->editWindow);
}

