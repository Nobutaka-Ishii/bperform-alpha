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
extern snd_seq_t *handle;


void varEdit(GtkWidget* button, effectStrip_t* varStripp)
{
	GtkWidget* paramLabel;

	paramLabel = gtk_label_new(varStripp->effectInfo->currentInsType);
	gtk_box_pack_start( GTK_BOX(varStripp->editWindowBox), paramLabel, TRUE, 0, 0);
	gtk_window_set_default_size(GTK_WINDOW(varStripp->editWindow), 200, 400);
	//gtk_widget_set_sensitive(varStripp->insEditButton, FALSE);
	gtk_widget_show_all(varStripp->editWindow);
}

void insEdit(GtkWidget* button, effectStrip_t* stripp)
{
	GtkWidget* paramLabel;

	paramLabel = gtk_label_new(stripp->effectInfo->currentInsType);
	gtk_box_pack_start( GTK_BOX(stripp->editWindowBox), paramLabel, TRUE, 0, 0);
	gtk_window_set_default_size(GTK_WINDOW(stripp->editWindow), 200, 400);
	//gtk_widget_set_sensitive(stripp->insEditButton, FALSE);
	gtk_widget_show_all(stripp->editWindow);
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

void varTypeSelected( GtkWidget* combo, effects_t* varp)
{
	gchar *eName; // selected effect name from the combo box entries.
	GList* list = varp->effectList;

	eName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	while( list ){
		if( !strcmp(eName, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	} 
	strcpy(varp->currentInsType, ((eachEffect_t*)(list->data))->name);

	if( strcmp("null", ((eachEffect_t*)list->data)->param[9].label) ){
		gtk_widget_set_sensitive(varp->range, TRUE);
		gtk_range_set_range(GTK_RANGE(varp->range),
			((eachEffect_t*)list->data)->param[9].rangeMin,
			((eachEffect_t*)list->data)->param[9].rangeMax);
		sendExc(5 , 0x02, 0x01, 0x40, ((eachEffect_t*)list->data)->msb,\
			((eachEffect_t*)list->data)->lsb);
	} else {
		gtk_widget_set_sensitive(varp->range, FALSE);
		gtk_range_set_range(GTK_RANGE(varp->range), 0, 0);
	}
}

//void effectTypeSelected( GtkWidget* combo, effects_t* effectp)
void effectTypeSelected( GtkWidget* combo, effectStrip_t* stripp)
{
	gchar *eName; // selected effect name from the combo box entries.
	//GList* list = effectp->effectList;
	GList* list = stripp->effectInfo->effectList;

	eName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	while( list ){
		if( !strcmp(eName, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	} 
	strcpy(stripp->effectInfo->currentInsType, ((eachEffect_t*)(list->data))->name);

	if( strcmp("null", ((eachEffect_t*)list->data)->param[9].label) ){
		gtk_widget_set_sensitive(stripp->effectInfo->range, TRUE);
		gtk_range_set_range(GTK_RANGE(stripp->effectInfo->range),
			((eachEffect_t*)list->data)->param[9].rangeMin,
			((eachEffect_t*)list->data)->param[9].rangeMax);

		switch( stripp->whichstrip ) {
			case INS0:
				sendExc(5 , 0x03, 0x00, 0x00, ((eachEffect_t*)list->data)->msb,\
					((eachEffect_t*)list->data)->lsb);
				break;
			case INS1:
				sendExc(5 , 0x03, 0x01, 0x00, ((eachEffect_t*)list->data)->msb,\
					((eachEffect_t*)list->data)->lsb);
				break;
			case VAR:
				sendExc(5 , 0x02, 0x01, 0x40, ((eachEffect_t*)list->data)->msb,\
					((eachEffect_t*)list->data)->lsb);
				break;
		}
	} else {
		gtk_widget_set_sensitive(stripp->effectInfo->range, FALSE);
		gtk_range_set_range(GTK_RANGE(stripp->effectInfo->range), 0, 0);
	}
}

void varTargetChnlSelected( GtkWidget* combo)
{
	gchar *chnl;
	chnl = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	if (!strcmp(chnl, "System" ) ){
		sendExc(4 , 0x02, 0x01, 0x5a, 0x1); // system
		sendExc(4 , 0x02, 0x01, 0x5b, 0x40);
	} else {
		sendExc(4 , 0x02, 0x01, 0x5a, 0x0); // insertion

		if (!strcmp(chnl, "Off" ) ){
			sendExc(4 , 0x02, 0x01, 0x5b, 0x7f);
		} else if (!strcmp(chnl, "1" ) ){
			sendExc(4 , 0x02, 0x01, 0x5b, 0x0);
		} else if (!strcmp(chnl, "2" ) ){
			sendExc(4 , 0x02, 0x01, 0x5b, 0x1);
		} else if (!strcmp(chnl, "3" ) ){
			sendExc(4 , 0x02, 0x01, 0x5b, 0x2);
		} else if (!strcmp(chnl, "4" ) ){
			sendExc(4 , 0x02, 0x01, 0x5b, 0x3);
		} else if (!strcmp(chnl, "AD" ) ){
			sendExc(4 , 0x02, 0x01, 0x5b, 0x40);
		}
	}
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

void closeEditWindow(GtkWidget* window, effectStrip_t* effectStripp)
{
	//gtk_widget_set_sensitive(effectStripp->insEditButton, FALSE);
	gtk_widget_hide(window);
}

