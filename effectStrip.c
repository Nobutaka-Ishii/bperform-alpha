#include <bperform.h>
#include <effectStrip.h>

effectStrip_t* effectStripConstr(gchar* stripName, gchar* path)
{
	FILE* fp;
	effectStrip_t* es;
	GtkWidget* effectBox;
	GtkWidget* typeComboBox;
	GtkWidget* chnlComboBox;
	GtkWidget* scale;
	GtkWidget* label;
	GtkWidget* editButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;

	es = (effectStrip_t*)malloc(sizeof(effectStrip_t));
	memset(es, 0, sizeof(effectStrip_t));

	effectBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	chnlComboBox = gtk_combo_box_text_new();
	typeComboBox = gtk_combo_box_text_new();
	scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);
	label = gtk_label_new(stripName);
	editButton = gtk_button_new_with_label("Edit");
	editWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	editWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	fp = fopen(path, "r");
	es->effectList = prepEffects(fp);
	fclose(fp);

	es->effectBox = effectBox;
	es->chnlComboBox = chnlComboBox;
	es->typeComboBox = typeComboBox;
	es->scale = scale;
	es->label = label;
	es->editButton = editButton;
	es->editWindow = editWindowBox;
	es->editWindowBox = editWindowBox;
	strcpy(es->currentEffectType, "Through");

	gtk_container_add( GTK_CONTAINER(editWindow), editWindowBox);

	gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(scale), TRUE);

	gtk_box_pack_start( GTK_BOX(effectBox), label, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), chnlComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), typeComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), scale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), editButton, FALSE, TRUE, 0);


	return es;
}

void insChanged(GtkRange* range, effectStrip_t* strip)
{
	GList* list = strip->effectList;
	guint val = gtk_range_get_value(range);
	do{
		if( !strcmp(strip->currentEffectType, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}while( list );
	if( ((eachEffect_t*)list->data)->addrWidth == 2 ){
		// this type of effect need 2-bytes width prameter specification.
		sendExc(5, 0x03, 0x00, 0x42, 0x00, val);
	}else{
		sendExc(4, 0x03, 0x00, 0x0B, val);
	}
}
 
void createEffectTypeComboBox(GtkWidget* comboBox, effectStrip_t* es)
{
	GList* list = es->effectList;
	do {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox),\
			((eachEffect_t*)(list->data))->name);
		list = list->next;
	} while( list );
}

static void createTargetChnlComboBox(GtkWidget* comboBox, effectStrip_t* es)
{
	int itr;
	for( itr = 0; itr < 4;  itr++)
	{
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), "hoge");
	}
}
 
