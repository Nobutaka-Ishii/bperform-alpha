#include <gtk/gtk.h>
#include "bperform.h"
#include <alsa/asoundlib.h>
#include <alsa/seq.h>
#include <alsa/seq_event.h>
#include <alsa/seqmid.h>
#include <alsa/seq_midi_event.h>

extern int source; // source alsa-client id;
extern int sport; // app's source MIDI port number
extern int tport; // target client's midi port number
extern int dstMaxEntries;
extern struct _midiTarget midiTargets[10];
extern snd_seq_t *handle;
extern int portaEnabled;
extern int monoEnabled;

void ac1intensityChanged(ac1_t* ac1){
}

void ac1menuSelected(ac1_t* ac1p){
	gchar* str;
	str = gtk_label_get_text( GTK_LABEL(ac1p->label)) ;
	g_print("%s\n", str);
}

void ins0edit(GtkWidget* button, insStrip_t* ins0stripp)
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

void ins0typeSelected( GtkWidget* combo, effects_t* ins0p)
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

void ins1typeSelected( GtkWidget* combo, effects_t* ins1p)
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
	} else if (!strcmp(chnl, "AD" ) ){
		sendExc(4 , 0x03, 0x00, 0x0c, 0x40);
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
	} else if (!strcmp(chnl, "AD" ) ){
		sendExc(4 , 0x03, 0x01, 0x0c, 0x40);
	}
}

void portaCheckBoxChecked(void)
{
	if(!portaEnabled){
		sendCc(PORTACCNUM, 127);
	}else{
		sendCc(PORTACCNUM, 0);
	}

	portaEnabled = !portaEnabled;
}

void monoCheckBoxChecked(void)
{
	if(!monoEnabled){
		sendExc(4, 0x08, 0x00, 0x5, 0x00);
	}else{
		sendExc(4, 0x08, 0x00, 0x5, 0x01);
	}

	monoEnabled = !monoEnabled;
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

gboolean quit_button_pushed
(GtkWidget* widget, GdkEvent *event, gpointer data)
{
	snd_seq_disconnect_to(handle, sport, source, tport);
	gtk_main_quit();
	return TRUE;
}

void reverbsend( GtkRange *range, GdkEvent *event, gpointer data)
{
    static guint cc = 91;
    guint val = (guint) gtk_range_get_value(range);
    sendCc(cc, val); // reverb send
    //g_print("%d\n", val);
}

void closeEditWindow(void){
}

