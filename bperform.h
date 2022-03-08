#include <gtk/gtk.h>
#include <ac1.h>
#include <voicePage.h>
#define PORTACCNUM 65

typedef struct _toneEntry toneEntry;

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

struct _eachEffect {
	gchar* name;
	guint msb;
	guint lsb;
	guint addrWidth; // some effects needs 2 byte with parameter value specification.
	param param[16]; // MU100B's each effects has max 16 parameters.
};

typedef struct _eachEffect eachEffect_t;

struct _effects {
	GtkWidget* range; // used for slider range changing operation.
	GList* effectList; // effectList is the set of many insertion effects.
	gchar currentInsType[64];
};

typedef struct _effects effects_t;

enum _strip_instance {
	INS0, INS1, VAR
};
typedef enum _strip_instance strip_instance_t;

struct _effectStrip {
	GtkWidget* insertBox;
	GtkWidget* insTargetChnl;
	GtkWidget* insType;
	GtkWidget* insScale;
	GtkWidget* insLabel;
	GtkWidget* insEditButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;
	effects_t* effectInfo;
	strip_instance_t whichstrip;
};

typedef struct _effectStrip effectStrip_t;

void pgmChange(int pn);
void sendExc(guint length,...);
void sendCc(guint cc, guint val);
void init_synth(void);
void revSend(GtkRange* range);
void choSend(GtkRange* range);
void volChanged(GtkRange* range);
void attackChanged(GtkRange* range);
void releaseChanged(GtkRange* range);
void decayChanged(GtkRange* range);
void varChanged(GtkRange* range, effects_t* varp);
void ins0changed(GtkRange* range, effects_t* ins0p);
void ins1changed(GtkRange* range, effects_t* ins1p);
void prepEffects(effects_t* effects, FILE* fp);
void stereoInitSelected(void);
void monauralInitSelected(void);
void varEdit(GtkWidget* button, effectStrip_t* varStripp);
void insEdit(GtkWidget* button, effectStrip_t* stripp);
void targetMidiPortSelected( GtkWidget *checkMenu, midiTarget_t* midiTarget_p);
void varTypeSelected( GtkWidget* combo, effects_t* varp);
void effectTypeSelected( GtkWidget* combo, effectStrip_t* stripp);
void varTargetChnlSelected( GtkWidget* combo);
void ins0targetChnlSelected( GtkWidget* combo);
void ins1targetChnlSelected( GtkWidget* combo);
void monoCheckBoxChecked(GtkWidget* checkbutton, monoInst_t* monoInst);
void portaCheckBoxChecked(GtkWidget* checkbutton, portaInst_t* portaInst);
void portaTimeChanged(GtkWidget* scale);
// void programSelected(GtkWidget* pListComboBox, GList** toneEntries);
gboolean delete_event (void);
gboolean quit_button_pushed (GtkWidget* widget, GdkEvent *event, gpointer data);
void destroy(void);
void reverbsend( GtkRange* range, GdkEvent* event, gpointer data);
void closeEditWindow(GtkWidget* window, effectStrip_t* effectStripp);
char** splitline(char* original, char delim, int* fields);

