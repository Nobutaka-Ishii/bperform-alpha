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
extern snd_seq_t *handle;

void ac1okButtonClicked(GtkWidget* button, ac1_t* ac1p)
{
	ac1p->cc = ac1p->tmpCc;
	ac1p->intensity = ac1p->tmpIntensity;
	sendExc(4, 0x03, 0x00, 0x10, ac1p->intensity);
	sendExc(4, 0x08, 0x00, 0x59, ac1p->cc);
	gtk_widget_hide( ac1p->window );
}

void ac1intensityChanged(GtkWidget* scale, ac1_t* ac1p)
{
	gint val = gtk_range_get_value( GTK_RANGE(scale) ) + 64;
	ac1p->tmpIntensity = val;
}

void ac1ccChanged(GtkWidget* spinbutton, ac1_t* ac1p)
{
	guint val = gtk_spin_button_get_value( GTK_SPIN_BUTTON(spinbutton) );
	ac1p->tmpCc = val;
}

void ac1menuSelected(GtkWidget* menuButton, ac1_t* ac1p){
	gtk_widget_show_all(ac1p->window);
}

void ins0edit(GtkWidget* button, insStrip_t* ins0stripp)
{
	GtkWidget* paramLabel;

	paramLabel = gtk_label_new(ins0stripp->effectInfo->currentInsType);
	gtk_box_pack_start( GTK_BOX(ins0stripp->editWindowBox), paramLabel, TRUE, 0, 0);
	gtk_window_set_default_size(GTK_WINDOW(ins0stripp->editWindow), 200, 400);
	//gtk_widget_set_sensitive(ins0stripp->insEditButton, FALSE);
	gtk_widget_show_all(ins0stripp->editWindow);
}


void targetMidiPortSelected( GtkWidget *label, midiTarget_t* midiTarget_p){
	if( midiTarget_p->checked){
		snd_seq_disconnect_to(handle, 0, midiTarget_p->clientId, midiTarget_p->portId);
		midiTarget_p->checked = 0;
	} else {
		snd_seq_connect_to(handle, 0, midiTarget_p->clientId, midiTarget_p->portId);
		midiTarget_p->checked = 1;
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

void portaCheckBoxChecked(GtkWidget* checkbutton, portaInst_t* portaInst)
{
	if(!portaInst->portaEnabled){
		sendCc(PORTACCNUM, 127);
	}else{
		sendCc(PORTACCNUM, 0);
	}

	portaInst->portaEnabled = !portaInst->portaEnabled;
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

void closeEditWindow(GtkWidget* window, insStrip_t* insStripp)
{
	//gtk_widget_set_sensitive(insStripp->insEditButton, FALSE);
	gtk_widget_hide(window);
}

