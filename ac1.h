#include <gtk/gtk.h>

struct _ac1 {
	GtkWidget* window;
	GtkWidget* ac1CcSpinbutton; // cc value spinbutton
	GtkWidget* ac1intensityScale; // intensity value range object
	GtkWidget* label;
	guint   cc;
	guint   tmpCc;
	gint    intensity;
	gint    tmpIntensity;
	struct _ac1*(*ac1constr)(void);
	void (*ac1menuSelected)(GtkWidget* menuButton, struct _ac1* ac1p);
};

typedef struct _ac1 ac1_t;

ac1_t* ac1constr(void);
void ac1okButtonClicked(GtkWidget* button, ac1_t* ac1p);
void ac1intensityChanged(GtkWidget* scale, ac1_t* ac1p);
void ac1ccChanged(GtkWidget* spinbutton, ac1_t* ac1p);
void ac1menuSelected(GtkWidget* menuButton, ac1_t* ac1p);

