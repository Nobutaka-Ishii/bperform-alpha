#include <gtk/gtk.h>

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

typedef struct _effects effects;

struct _insStrip {
	GtkWidget* insertBox;
	GtkWidget* insTargetChnl;
	GtkWidget* insType;
	GtkWidget* insScale;
	GtkWidget* insLabel;
	GtkWidget* insEditButton;
	GtkWidget* editWindow;
	effects* effectInfo;
};

typedef struct _insStrip insStrip;

void pgmChange(int pn);
void sendExc(guint length,...);
void sendCc(guint cc, guint val);
void init_synth(void);
void revSend(GtkRange* range);
void choSend(GtkRange* range);
void attackChanged(GtkRange* range);
void releaseChanged(GtkRange* range);
void decayChanged(GtkRange* range);
void ins0changed(GtkRange* range, effects* ins0p);
void ins1changed(GtkRange* range, effects* ins1p);
void prepIns(effects* ins);

