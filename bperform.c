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

snd_seq_t *handle;
int source; // source alsa-client id;
int sport = 0; // app's source MIDI port number
int tport = 0; // target client's midi port number

void initializeSelected(GtkWidget* menu)
{
	// XG reset
	sendExc(4, 0x4c, 0x00, 0x7f, 0x00);
}


void monoCheckBoxChecked(GtkWidget* checkbutton, monoInst_t* monoInst)
{
	if(!monoInst->monoEnabled){
		sendExc(4, 0x08, 0x00, 0x5, 0x00);
	}else{
		sendExc(4, 0x08, 0x00, 0x5, 0x01);
	}
	monoInst->monoEnabled = !monoInst->monoEnabled;
}

void createEffectTypeComboBox(GtkWidget* comboBox, effects_t* effectsp)
{
	GList* list = effectsp->effectList;

	do {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox),\
			((eachEffect_t*)(list->data))->name);
		list = list->next;
	} while( list );

}

gchar* chnlInsComboBoxEntries[] = {"Off", "1", "2", "3", "4", "AD"};
gchar* chnlVarComboBoxEntries[] = {"Off", "1", "2", "3", "4", "AD", "System"};

static void createInsTargetChnlComboBox(GtkWidget* comboBox)
{
	int itr;
	for( itr = 0; itr < sizeof(chnlInsComboBoxEntries) / sizeof(*chnlInsComboBoxEntries) ; itr++){
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox),\
			chnlInsComboBoxEntries[itr]);
	}
}

static void createVarTargetComboBox(GtkWidget* comboBox)
{
	int itr;
	for( itr = 0; itr < sizeof(chnlVarComboBoxEntries) / sizeof(*chnlVarComboBoxEntries) ; itr++){
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox),\
			chnlVarComboBoxEntries[itr]);
	}
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

int main(int argc, char** argv)
{
	FILE* fp;
	effects_t var;
	effects_t ins0;
	effects_t ins1;
	effectStrip_t varStrip;
	effectStrip_t ins0strip;
	effectStrip_t ins1strip;
	ac1_t* ac1p;
	voicePage_t* voicePage0p;
	GList* midiTargets = NULL;

		// variables for alsa connection destination port
	snd_seq_client_info_t *cinfo;
	snd_seq_port_info_t *pinfo;

		// preparation of dst port searching
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_set_client(cinfo, -1);

		// main window instances
	GtkWidget* window;
	GtkWidget* base; // acommodate menubar on top and others on bottom
	GtkWidget* menubar; // occupies top place
	GtkWidget* exceptMenu; // occupies button place
	GtkWidget* ac1menu;


	GtkWidget* voicePages;

	// page1 entries
	GtkWidget* voicePage1;
	//GtkWidget* page1contents;


	// system effect entries
	GtkWidget* variationBox;
	GtkWidget* varTargetChnl;
	GtkWidget* varType;
	GtkWidget* varScale;
	GtkWidget* varLabel;
	GtkWidget* varEditButton;
	GtkWidget* varEditWindow;
	GtkWidget* varEditWindowBox;

	GtkWidget* insert0box;
	GtkWidget* ins0type;
	GtkWidget* ins0scale;
	GtkWidget* ins0targetChnl;
	GtkWidget* ins0label;
	GtkWidget* ins0editButton;
	GtkWidget* ins0editWindow;
	GtkWidget* ins0editWindowBox;

	GtkWidget* insert1box;
	GtkWidget* ins1type;
	GtkWidget* ins1scale;
	GtkWidget* ins1targetChnl;
	GtkWidget* ins1label;
	GtkWidget* ins1editButton;
	GtkWidget* ins1editWindow;
	GtkWidget* ins1editWindowBox;

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
	GtkWidget* monauralInit;
	GtkWidget* stereoInit; 

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

		// preparation for using glib list methods.
	var.effectList = NULL;
	ins0.effectList = NULL;
	ins1.effectList = NULL;

		// main window and layouts
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	base = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

		// edit window and layouts
	ins0editWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ins1editWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	varEditWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

		// ac1 window and layouts instance generation
	ac1p = ac1constr();
	voicePage0p = voicePageConstr();


	// create menubar bar
	menubar = gtk_menu_bar_new();

	initDownlist = gtk_menu_new();
	init = gtk_menu_item_new_with_label("Initialize");
	initialize = gtk_menu_item_new_with_label("MU100B init");
	monauralInit = gtk_menu_item_new_with_label("Mono AD init");
	stereoInit = gtk_menu_item_new_with_label("Stereo AD init");
	ac1menu = gtk_menu_item_new_with_label("AC1 config");

	connectDownlist = gtk_menu_new();
	connect = gtk_menu_item_new_with_label("Connect");

	helpDownlist = gtk_menu_new();
	help = gtk_menu_item_new_with_label("Help");
	quit = gtk_menu_item_new_with_label("Quit");

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), init);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(init), initDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), initialize);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), monauralInit);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), stereoInit);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), ac1menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), connect);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(connect), connectDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpDownlist), quit);

	// midi connection target listing up
    while (snd_seq_query_next_client(handle, cinfo) >= 0) {

		midiTarget_t* midiTarget_p;
		char midiTargetName[128]; // used for midi taget name construction
        char strTmp[64]; // used for midi taget name construction

        /* reset query info */
        snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
        snd_seq_port_info_set_port(pinfo, -1);

		while (snd_seq_query_next_port(handle, pinfo) >= 0) {
			midiTarget_p = (midiTarget_t*)malloc( sizeof(midiTarget_t) );
			midiTarget_p->clientId = snd_seq_client_info_get_client(cinfo);
			midiTarget_p->portId = snd_seq_port_info_get_port(pinfo);
			strcpy( midiTarget_p->clientName, snd_seq_port_info_get_name(pinfo) );
			midiTarget_p->checked = 0;

			// construction of entry name
			memset(midiTargetName, 0, 128);
			strcat(midiTargetName, midiTarget_p->clientName);

			memset(strTmp, 0, 64);
			strcat(midiTargetName, ":\t");
			strcat(midiTargetName, strTmp);

			memset(strTmp, 0, 64);
			sprintf(strTmp, "%d", midiTarget_p->clientId);
			strcat(midiTargetName, strTmp);
			strcat(midiTargetName, " / ");

			memset(strTmp, 0, 64);
			sprintf(strTmp, "%d", midiTarget_p->portId);
			strcat(midiTargetName, strTmp);

			midiTargets = g_list_append(midiTargets, midiTarget_p);

			midiTarget_p->checkMenu = gtk_check_menu_item_new_with_label(midiTargetName);
			gtk_menu_shell_append(GTK_MENU_SHELL(connectDownlist), midiTarget_p->checkMenu) ;

		}
	}

	
	// create area other than menubar bar
	exceptMenu = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	//voicePage0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	voicePage1 = gtk_button_new_with_label("hoge");

	voicePages = gtk_notebook_new();
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voicePage0p->voicePage , GTK_WIDGET(gtk_label_new("Voice0") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voicePage1, GTK_WIDGET(gtk_label_new("Voice1") ) );

/*
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voiceBox, GTK_WIDGET(gtk_label_new("Voice2") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voiceBox, GTK_WIDGET(gtk_label_new("Voice3") ) );
*/

		// variation strip
	variationBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	varEditWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	varLabel = gtk_label_new("Variation");
	varEditButton = gtk_button_new_with_label("Edit");
	varScale= gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
	varTargetChnl = gtk_combo_box_text_new();
	varType = gtk_combo_box_text_new();
	var.range = varScale;
	fp = fopen("./varList.txt", "r");
	prepEffects(&var, fp);
	fclose(fp);
	createVarTargetComboBox(varTargetChnl);
	createEffectTypeComboBox(varType, &var);
		// var object construction
	varStrip.insertBox = variationBox;
	varStrip.insLabel = varLabel;
	varStrip.insEditButton = varEditButton;
	varStrip.insScale = varScale;
	varStrip.insTargetChnl = varTargetChnl;
	varStrip.effectInfo = &var;
	gtk_container_add( GTK_CONTAINER(varEditWindow), varEditWindowBox);
	varStrip.editWindow = varEditWindow;
	varStrip.editWindowBox = varEditWindowBox; 
	varStrip.whichstrip = VAR;

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

		// insert0 strip
	insert0box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		// edit subwindow related to ins0 strip
	ins0editWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ins0label = gtk_label_new("Insert1");
	ins0editButton = gtk_button_new_with_label("Edit");
	ins0scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);
		// create combobox entries with insertion effect type initialization
	ins0targetChnl = gtk_combo_box_text_new();
	ins0type = gtk_combo_box_text_new();
	ins0.range = ins0scale;
	fp = fopen("./insList.txt", "r");
	prepEffects(&ins0, fp);
	fclose(fp);
		// create combobox entries of effect target channel
	createInsTargetChnlComboBox(ins0targetChnl);
	createEffectTypeComboBox(ins0type, &ins0);
		// ins0 object construction
	ins0strip.insertBox = insert0box;
	ins0strip.insLabel = ins0label;
	ins0strip.insEditButton = ins0editButton;
	ins0strip.insScale = ins0scale;
	ins0strip.insTargetChnl = ins0targetChnl;
	ins0strip.effectInfo = &ins0;
	gtk_container_add( GTK_CONTAINER(ins0editWindow), ins0editWindowBox);
	ins0strip.editWindow = ins0editWindow;
	ins0strip.editWindowBox = ins0editWindowBox; 
	ins0strip.whichstrip = INS0;

		// insert1 strip
	insert1box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		// edit subwindow related to ins1 strip
	ins1editWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ins1label = gtk_label_new("Insert2");
	ins1editButton = gtk_button_new_with_label("Edit");
	ins1scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);

	ins1targetChnl = gtk_combo_box_text_new();
	ins1type = gtk_combo_box_text_new();
	ins1.range = ins1scale;
	fp = fopen("./insList.txt", "r");
	prepEffects(&ins1, fp);
	fclose(fp);
		// create 
	createInsTargetChnlComboBox(ins1targetChnl);
	createEffectTypeComboBox(ins1type, &ins1);
		//ins1 object construction
	ins1strip.insertBox = insert0box;
	ins1strip.insLabel = ins1label;
	ins1strip.insEditButton = ins1editButton;
	ins1strip.insScale = ins1scale;
	ins1strip.insTargetChnl = ins1targetChnl;
	ins1strip.effectInfo = &ins1;
	gtk_container_add( GTK_CONTAINER(ins1editWindow), ins1editWindowBox);
	ins1strip.editWindow = ins1editWindow;
	ins1strip.editWindowBox = ins1editWindowBox; 
	ins1strip.whichstrip = INS1;
	
		// mono/poly function
/*
	monoInst.checkBox = monoCheckBox;
	monoInst.monoEnabled = 0; // poly mode in default.

	portaInst.checkBox = portaCheckBox;
	portaInst.portaEnabled = 0;
	portaInst.scale = portaTimeScale;
*/

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

		// window events by window manager
	g_signal_connect(G_OBJECT(window), "delete_event",\
		G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",\
		G_CALLBACK(destroy), NULL);
	g_signal_connect(G_OBJECT(ac1menu), "activate", G_CALLBACK( ac1p->ac1menuSelected ), ac1p);


		// menu selection
	g_signal_connect(G_OBJECT(initialize), "activate",\
		G_CALLBACK(initializeSelected), NULL);
	g_signal_connect(G_OBJECT(monauralInit), "activate",\
		G_CALLBACK(monauralInitSelected), NULL);
	g_signal_connect(G_OBJECT(stereoInit), "activate",\
		G_CALLBACK(stereoInitSelected), NULL);
	g_signal_connect(G_OBJECT(quit), "activate",\
		G_CALLBACK(quit_button_pushed), NULL);

		// variation effect strip
	g_signal_connect(G_OBJECT(varType), "changed", \
		G_CALLBACK(varTypeSelected), &var);
	g_signal_connect(G_OBJECT(varScale), "value-changed", \
		G_CALLBACK(varChanged), &var);
	g_signal_connect(G_OBJECT(varTargetChnl), "changed", \
		G_CALLBACK(varTargetChnlSelected), NULL);
	g_signal_connect(G_OBJECT(varEditButton), "clicked", \
		G_CALLBACK(varEdit), &varStrip);
	g_signal_connect(G_OBJECT(varEditWindow), "delete_event", \
		G_CALLBACK(closeEditWindow), &varStrip);

		// insertion effect strips
	g_signal_connect(G_OBJECT(ins0type), "changed", \
		G_CALLBACK(effectTypeSelected), &ins0strip);
	g_signal_connect(G_OBJECT(ins0scale), "value-changed", \
		G_CALLBACK(ins0changed), &ins0);
	g_signal_connect(G_OBJECT(ins0targetChnl), "changed", \
		G_CALLBACK(ins0targetChnlSelected), NULL);
	g_signal_connect(G_OBJECT(ins0editButton), "clicked", \
		G_CALLBACK(insEdit), &ins0strip);
	g_signal_connect(G_OBJECT(ins0editWindow), "delete_event", \
		G_CALLBACK(closeEditWindow), &ins0strip);

	g_signal_connect(G_OBJECT(ins1type), "changed", \
		G_CALLBACK(effectTypeSelected), &ins1strip);
	g_signal_connect(G_OBJECT(ins1scale), "value-changed", \
		G_CALLBACK(ins1changed), &ins1);
	g_signal_connect(G_OBJECT(ins1targetChnl), "changed", \
		G_CALLBACK(ins1targetChnlSelected), NULL);
	g_signal_connect(G_OBJECT(ins1editButton), "clicked", \
		G_CALLBACK(insEdit), &ins1strip);
	g_signal_connect(G_OBJECT(ins1editWindow), "delete_event", \
		G_CALLBACK(closeEditWindow), &ins1strip);

		// midi connection button actions
	midiTargets = g_list_first(midiTargets);
	while(midiTargets->next){
		g_signal_connect( \
			G_OBJECT( (GtkWidget*)( ((midiTarget_t*)(midiTargets->data))->checkMenu) ), \
			"activate", G_CALLBACK(targetMidiPortSelected), midiTargets->data);
		midiTargets = midiTargets->next;
	}


	// widgets boxing
/*
	gtk_box_pack_start( GTK_BOX(page0left), pListComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), volBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), panBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), attackBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), decayBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), releaseBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(page0left), monoCheckBox, FALSE, TRUE, 0);
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
	gtk_box_pack_start( GTK_BOX(choSendBox), choSendLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choSendBox), choSendScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revSendBox), revSendLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revSendBox), revSendScale, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(voicePage0), page0contents);
*/



	gtk_box_pack_start( GTK_BOX(variationBox), varLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(variationBox), varTargetChnl, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(variationBox), varType, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(variationBox), varScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(variationBox), varEditButton, FALSE, TRUE, 0);


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
	gtk_main();
	return 0;
}

