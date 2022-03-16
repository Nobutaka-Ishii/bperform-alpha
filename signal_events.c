#include <gtk/gtk.h>
#include <bperform.h>
#include <alsa/asoundlib.h>
#include <alsa/seq.h>
#include <alsa/seq_event.h>
#include <alsa/seqmid.h>
#include <alsa/seq_midi_event.h>
#include <effectStrip.h>

extern int source; // source alsa-client id;
extern int sport; // app's source MIDI port number
extern int tport; // target client's midi port number
extern snd_seq_t *handle;

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

