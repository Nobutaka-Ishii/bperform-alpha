#include <gtk/gtk.h>

struct _midiTarget{
	guint clientId;
	guint portId;
	gchar clientName[64];
	GtkWidget* checkMenu;
	guint checked;
};
typedef struct _midiTarget midiTarget_t;

void pgmChange(int pn);
void sendExc(guint length,...);
void sendCc(guint cc, guint val);
void init_synth(void);
void stereoInitSelected(void);
void monauralInitSelected(void);
void targetMidiPortSelected( GtkWidget *checkMenu, midiTarget_t* midiTarget_p);
gboolean delete_event (void);
gboolean quit_button_pushed (GtkWidget* widget, GdkEvent *event, gpointer data);
void destroy(void);
void reverbsend( GtkRange* range, GdkEvent* event, gpointer data);

enum {SYNTH, AD};

