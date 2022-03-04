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
	GtkWidget* checkMenu;
	guint checked;
};
typedef struct _midiTarget midiTarget_t;


struct _param{
	gchar* label;
	gint rangeMax;
	gint rangeMin;
};

typedef struct _param param;

struct _insEffect {
	gchar* name;
	guint msb;
	guint lsb;
	guint addrWidth; // some effects needs 2 byte with parameter value specification.
	param param[16]; // MU100B's each effects has max 16 parameters.
};

typedef struct _insEffect insEffect;

struct _effects {
	GtkWidget* range; // used for slider range changing operation.
	GList* effectList; // effectList is the set of many insertion effects.
	gchar currentInsType[64];
};

typedef struct _effects effects_t;

struct _monoInst {
	GtkWidget* checkBox;
	int monoEnabled; // 0:poly, 1:mono
	void (*toggleMono)(struct _monoInst* monoInst);
};
typedef struct _monoInst monoInst_t;

struct _portaInst {
	GtkWidget* checkBox;
	GtkWidget* label;
	GtkWidget* scale;
	guint portaEnabled;
	guint value;
	guint (*getPortaDuration)(struct _portaInst* portaInst);
};
typedef struct _portaInst portaInst_t;

struct _insStrip {
	GtkWidget* insertBox;
	GtkWidget* insTargetChnl;
	GtkWidget* insType;
	GtkWidget* insScale;
	GtkWidget* insLabel;
	GtkWidget* insEditButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;
	effects_t* effectInfo;
};

typedef struct _insStrip insStrip_t;

struct _ac1 {
	GtkWidget* window;
	GtkWidget* ccSpinbutton; // cc value spinbutton
	GtkWidget* intensityScale; // intensity value range object
	GtkWidget* label;
	guint	cc;
	guint	tmpCc;
	gint	intensity;
	gint	tmpIntensity;
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
void monauralInitSelected(void);
void ins0edit(GtkWidget* button, insStrip_t* ins0stripp);
void targetMidiPortSelected( GtkWidget *checkMenu, midiTarget_t* midiTarget_p);
void ins0typeSelected( GtkWidget* combo, effects_t* ins0p);
void ins1typeSelected( GtkWidget* combo, effects_t* ins1p);
void ins0targetChnlSelected( GtkWidget* combo);
void ins1targetChnlSelected( GtkWidget* combo);
void monoCheckBoxChecked(GtkWidget* checkbutton, monoInst_t* monoInst);
void portaCheckBoxChecked(GtkWidget* checkbutton, portaInst_t* portaInst);
void portaTimeChanged(GtkWidget* scale);
void programSelected(GtkWidget* pListComboBox, GList* toneEntries);
void ac1menuSelected(GtkWidget* menuButton, ac1_t* ac1p);
gboolean delete_event (void);
gboolean quit_button_pushed (GtkWidget* widget, GdkEvent *event, gpointer data);
void destroy(void);
void reverbsend( GtkRange* range, GdkEvent* event, gpointer data);
void closeEditWindow(GtkWidget* window, insStrip_t* insStripp);
void toggleMono(GtkWidget* checkbutton, monoInst_t* monoInst);
void ac1intensityChanged(GtkWidget* scale, ac1_t* ac1p);
void ac1ccChanged(GtkWidget* spinbutton, ac1_t* ac1p);
void ac1okButtonClicked(GtkWidget* button, ac1_t* ac1p);
char** splitline(char* original, char delim, int* fields);

