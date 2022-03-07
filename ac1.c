#include <bperform.h>

ac1_t* ac1constr(void)
{
	ac1_t* ac1p;
	GtkWidget* ac1window;
	GtkWidget* ac1base;
	GtkWidget* ac1boxUpper;
	GtkWidget* ac1boxMiddle;
	GtkWidget* ac1boxLower;
	GtkWidget* ac1label;
	GtkWidget* ac1okButton;
	GtkWidget* ac1CcSpinbutton;
	GtkWidget* ac1intensityScale;

	ac1p = (ac1_t*)malloc(sizeof(ac1_t));
	memset(ac1p, 0, sizeof(ac1_t));

	ac1okButton = gtk_button_new_with_label("OK");
	ac1window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	ac1base = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	ac1boxUpper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ac1boxMiddle = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ac1boxLower = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ac1label = gtk_label_new("CC#");
	ac1CcSpinbutton = gtk_spin_button_new_with_range(0, 95, 1); // for CC target number input
	ac1intensityScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -64, 63, 1);

	ac1p->window = ac1window;
	ac1p->ac1CcSpinbutton = ac1CcSpinbutton;
	ac1p->ac1intensityScale = ac1intensityScale;
	ac1p->label = ac1label;
	ac1p->cc = 45; // my default cc value
	ac1p->tmpCc = 45; // my default cc value
	ac1p->intensity = 63; // my default intensity value
	ac1p->tmpIntensity = 63; // my default intensity value
	ac1p->ac1menuSelected = ac1menuSelected;

	gtk_box_pack_start( GTK_BOX(ac1boxUpper), ac1label, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(ac1boxUpper), ac1CcSpinbutton, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(ac1boxMiddle), ac1intensityScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(ac1boxLower), ac1okButton, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(ac1base), ac1boxUpper, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(ac1base), ac1boxMiddle, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(ac1base), ac1boxLower, TRUE, 0, 0);
	gtk_container_add( GTK_CONTAINER(ac1window), ac1base);

		// ac1 window internal events
	g_signal_connect(G_OBJECT(ac1intensityScale), "value-changed", \
		G_CALLBACK(ac1intensityChanged), ac1p);
	g_signal_connect(G_OBJECT(ac1CcSpinbutton), "value-changed", \
		G_CALLBACK(ac1ccChanged), ac1p);
	g_signal_connect(G_OBJECT(ac1okButton), "clicked",\
		G_CALLBACK(ac1okButtonClicked), ac1p);

	return ac1p;

}

void ac1okButtonClicked(GtkWidget* button, ac1_t* ac1p)
{
	ac1p->cc = ac1p->tmpCc;
	ac1p->intensity = ac1p->tmpIntensity;
	sendExc(4, 0x03, 0x00, 0x10, ac1p->intensity);
	sendExc(4, 0x08, 0x00, 0x59, ac1p->cc);
	gtk_widget_hide( ac1p->window );
}

void ac1intensityChanged(GtkWidget* scale, ac1_t* ac1p)
{
	gint val = gtk_range_get_value( GTK_RANGE(scale) ) + 64;
	ac1p->tmpIntensity = val;
}

void ac1ccChanged(GtkWidget* spinbutton, ac1_t* ac1p)
{
	guint val = gtk_spin_button_get_value( GTK_SPIN_BUTTON(spinbutton) );
	ac1p->tmpCc = val;
}

void ac1menuSelected(GtkWidget* menuButton, ac1_t* ac1p)
{
	gtk_widget_show_all(ac1p->window);
}

