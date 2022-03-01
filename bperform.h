#include <gtk/gtk.h>
#define PORTACCNUM 65

struct _toneEntry {
	gchar name[16];
	char msb[4];
	char lsb[4];
	char pc[4];
};

typedef struct _toneEntry toneEntry;

struct _tones {
	GList* toneEntries;
};

typedef struct _tones tones;

struct _midiTarget{
	guint clientId;
	guint portId;
	gchar clientName[64];
	GtkWidget* instance;
	guint checked;
};

struct _param{
	gchar label[64];
	guint rangeMax;
	guint rangeMin;
};

typedef struct _param param;

struct _insEffect {
	gchar name[64];
	guint msb;
	guint lsb;
	guint addrWidth;
	param param1;
	param param2;
	param param3;
	param param4;
	param param5;
	param param6;
	param param7;
	param param8;
	param param9;
	param param10;
	param param11;
	param param12;
	param param13;
	param param14;
	param param15;
	param param16;
};

typedef struct _insEffect insEffect;

struct _effects {
	GtkWidget* range; // used for slider range changing operation.
	GList* effectList; // effectList is the set of many insertion effects.
	gchar currentInsType[64];
};

typedef struct _effects effects_t;

struct _insStrip {
	GtkWidget* insertBox;
	GtkWidget* insTargetChnl;
	GtkWidget* insType;
	GtkWidget* insScale;
	GtkWidget* insLabel;
	GtkWidget* insEditButton;
	GtkWidget* editWindow;
	effects_t* effectInfo;
};

typedef struct _insStrip insStrip_t;

struct _ac1 {
	GtkWidget* window;
	GtkWidget* combo;
	GtkWidget* scale;
	GtkWidget* label;
};

typedef struct _ac1 ac1_t;

void pgmChange(int pn);
void sendExc(guint length,...);
void sendCc(guint cc, guint val);
void init_synth(void);
void revSend(GtkRange* range);
void choSend(GtkRange* range);
void attackChanged(GtkRange* range);
void releaseChanged(GtkRange* range);
void decayChanged(GtkRange* range);
void ins0changed(GtkRange* range, effects_t* ins0p);
void ins1changed(GtkRange* range, effects_t* ins1p);
void prepIns(effects_t* ins);
void stereoInitSelected(void);
void monoInitSelected(void);
void ins0edit(GtkWidget* button, insStrip_t* ins0stripp);
void targetMidiPortSelected( GtkWidget *label);
void ins0typeSelected( GtkWidget* combo, effects_t* ins0p);
void ins1typeSelected( GtkWidget* combo, effects_t* ins1p);
void ins0targetChnlSelected( GtkWidget* combo);
void ins1targetChnlSelected( GtkWidget* combo);
void portaCheckBoxChecked(void);
void monoCheckBoxChecked(void);
void portaTimeChanged(GtkWidget* scale);
void programSelected(GtkWidget* pListComboBox, GList* toneEntries);
void ac1intensityChanged(ac1_t* ac1p);
void ac1menuSelected(ac1_t* ac1p);
gboolean delete_event (void);
gboolean quit_button_pushed (GtkWidget* widget, GdkEvent *event, gpointer data);
void destroy(void);
void reverbsend( GtkRange *range, GdkEvent *event, gpointer data);
void closeEditWindow(void);
