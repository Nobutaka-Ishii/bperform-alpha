#include <gtk/gtk.h>
#define EFFECT_NAME_LENGTH 64

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

struct _effectStrip {
	GtkWidget* effectBox; // this is the one to be packed on the main window
	GtkWidget* chnlComboBox;
	GtkWidget* typeComboBox;
	GtkWidget* scale;
	GtkWidget* label;
	GtkWidget* editButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;
	GList* effectList;
	gchar currentEffectType[EFFECT_NAME_LENGTH];

	struct _effectStrip* (*effectStripConstr)(void);
};

typedef struct _effectStrip effectStrip_t;

effectStrip_t* effectStripConstr(gchar* stripName, gchar* path); // path is effect list text.
GList* prepEffects(FILE* fp);

