#include <gtk/gtk.h>
#define EFFECT_NAME_LENGTH 64
#define PARAM_LABEL_NAME_LENGTH 32
#define MU100_EFFECT_PARAMS 16
#define EFFECT_STRIP_TYPE_NAME_LENGTH 16

struct _param{
	gchar label[PARAM_LABEL_NAME_LENGTH];
	gint rangeMax;
	gint rangeMin;
};
typedef struct _param param_t;

struct _eachEffect {
	gchar name[EFFECT_NAME_LENGTH];
	guint msb;
	guint lsb;
	guint addrWidth; // some effects needs 2 byte width parameter value specification.
	param_t param[MU100_EFFECT_PARAMS];
	gchar mainParam[PARAM_LABEL_NAME_LENGTH];
};
typedef struct _eachEffect eachEffect_t;

struct _eachParamStrip {
	GtkWidget* paramLabel;
	GtkWidget* paramBox;
	GtkWidget* paramScale;
};
typedef struct _eachParamStrip eachParamStrip_t;

struct _effectStrip {
	GtkWidget* effectStripBox; // this is the one to be packed on the main window
	GtkWidget* chnlComboBox;
	GtkWidget* effectTypeComboBox;
	GtkWidget* scale;
	GtkWidget* mainParamLabel;
	GtkWidget* label;
	GtkWidget* editButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;
	GtkWidget* paramEditFixButton;
	GtkWidget* paramEditFixStrip;
	GList* effectList;
	gchar stripName[EFFECT_STRIP_TYPE_NAME_LENGTH];
	eachParamStrip_t* paramStrips[MU100_EFFECT_PARAMS];
	eachEffect_t currentEffect;
	guint currentTargetChnl;

	struct _effectStrip* (*effectStripConstr)(void);
};

typedef struct _effectStrip effectStrip_t;

effectStrip_t* effectStripConstr(gchar* stripName, gchar* path); // path is effect list text.
void paramScaleTouchFunc(GtkWidget* scale, void* es);
GList* prepEffects(int fd);

