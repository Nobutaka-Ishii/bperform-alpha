#include <gtk/gtk.h>
#define EFFECT_NAME_LENGTH 64
#define PARAM_LABEL_NAME_LENGTH 32
#define MU100_EFFECT_PARAMS 16

struct _param{
	gchar *label;
	gint rangeMax;
	gint rangeMin;
};
typedef struct _param param;

struct _eachEffect {
	gchar* name;
	guint msb;
	guint lsb;
	guint addrWidth; // some effects needs 2 byte with parameter value specification.
	param param[MU100_EFFECT_PARAMS];
};
typedef struct _eachEffect eachEffect_t;

struct _effectStrip {
	GtkWidget* effectBox; // this is the one to be packed on the main window
	GtkWidget* chnlComboBox;
	GtkWidget* typeComboBox;
	GtkWidget* scale;
	GtkWidget* label;
	GtkWidget* editButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;
	GtkWidget* paramEditFixButton;
	GtkWidget* paramEditFixStrip;
	GList* effectList;
	gchar stripName[16]; // effect strip itselves name i.e Insert1, Insert2, Variation
	GtkWidget** paramScales; // on edit window
	GtkWidget** paramLabels; // on edit window
	GtkWidget** paramBoxes; // on edit window
	eachEffect_t* currentEffect;
	guint currentTargetChnl;

	struct _effectStrip* (*effectStripConstr)(void);
};

typedef struct _effectStrip effectStrip_t;

effectStrip_t* effectStripConstr(gchar* stripName, gchar* path); // path is effect list text.
GList* prepEffects(FILE* fp);

