#include <bperform.h>
#include <effectStrip.h>
#include <splitline.h>
#include <stdlib.h>

char *targetChnl[] = {"Off,0x7f", "1,0x0", "2,0x1", "3,0x2", "4,0x3", "AD,0x40"};

void createEffectTypeComboBox(GtkWidget* comboBox, GList* list);
void createTargetChnlComboBox(GtkWidget* comboBox);
void chnlChanged(GtkWidget* combo, effectStrip_t* es);
void effectTypeChanged(GtkWidget* combo, effectStrip_t* es);
void effectScaleChanged(GtkRange* range, effectStrip_t* es);

effectStrip_t* effectStripConstr(gchar* stripName, gchar* path)
{
	FILE* fp;
	effectStrip_t* es;
	GtkWidget* effectBox;
	GtkWidget* chnlComboBox;
	GtkWidget* typeComboBox;
	GtkWidget* scale;
	GtkWidget* label;
	GtkWidget* editButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;

	es = (effectStrip_t*)malloc(sizeof(effectStrip_t));
	memset(es, 0, sizeof(effectStrip_t));

	strcpy(es->stripName, stripName);

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
	es->currentTargetChnl = 0x7f; // targetChnl[0] = {"Off,0x7f"} 's entry.
	gtk_range_set_value( GTK_RANGE(es->scale), 1); // 1 is min value hard code for "Through" entry.

	// create channel combo box entries
	createTargetChnlComboBox(chnlComboBox);
		gtk_combo_box_set_active( GTK_COMBO_BOX(chnlComboBox), 0);
	createEffectTypeComboBox(typeComboBox, es->effectList);
		gtk_combo_box_set_active( GTK_COMBO_BOX(typeComboBox), 0);

	gtk_container_add( GTK_CONTAINER(editWindow), editWindowBox);

	gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(scale), TRUE);

	gtk_box_pack_start( GTK_BOX(effectBox), label, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), chnlComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), typeComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), scale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), editButton, FALSE, TRUE, 0);

	g_signal_connect(G_OBJECT(es->scale), "value-changed", G_CALLBACK(effectScaleChanged), es);
	g_signal_connect(G_OBJECT(es->chnlComboBox), "changed", G_CALLBACK(chnlChanged), es);
	g_signal_connect(G_OBJECT(es->typeComboBox), "changed", G_CALLBACK(effectTypeChanged), es);

	return es;
}

void chnlChanged(GtkWidget* combo, effectStrip_t* es)
{
	// getting insert effect target channel
	gchar* chnlName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );
	char** entry;
	guint val;
	int itr;
	int fields;

	for(itr = 0; itr < sizeof(targetChnl)/(sizeof(targetChnl[0])); itr++){
		entry = splitline(targetChnl[itr], ',', &fields);
		if( !strcmp(chnlName, entry[0]) ) break;
	}
	es->currentTargetChnl = val = (guint)strtod(entry[1], NULL);

	if( !strcmp("Insert1", es->stripName) ) sendExc(4, 0x03, 0x0, 0x0C, val);
	if( !strcmp("Insert2", es->stripName) )  sendExc(4, 0x03, 0x1, 0x0C, val);
	if( !strcmp("Variation", es->stripName) )  sendExc(4, 0x02, 0x1, 0x5B, val);

}

void effectTypeChanged(GtkWidget* combo, effectStrip_t* es)
{
	gchar *effectName; // selected effect name from the combo box entries.
	GList* list = es->effectList;
	guint msb;
	guint lsb;
	effectName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );
	while( list ){
		if( !strcmp(effectName, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}
	msb = ((eachEffect_t*)list->data)->msb;
	lsb = ((eachEffect_t*)list->data)->lsb;

	strcpy(es->currentEffectType, ((eachEffect_t*)(list->data))->name);

	if( strcmp("null", ((eachEffect_t*)list->data)->param[9].label) ){
		//gtk_widget_set_sensitive(es->scale, TRUE);
		gtk_range_set_range(GTK_RANGE(es->scale),
			((eachEffect_t*)list->data)->param[9].rangeMin,
			((eachEffect_t*)list->data)->param[9].rangeMax);
	} else {
		//gtk_widget_set_sensitive(es->scale, FALSE);
		//gtk_range_set_range(GTK_RANGE(es->scale), 0, 0);
	}

	if( !strcmp("Insert1", es->stripName) ){
		sendExc(5 , 0x03, 0x00, 0x00, msb, lsb);
	} else if( !strcmp("Insert2", es->stripName) ){
		sendExc(5 , 0x03, 0x01, 0x00, msb, lsb);
	} else {
		sendExc(5 , 0x02, 0x01, 0x40, msb, lsb);
	}
}

void effectScaleChanged(GtkRange* range, effectStrip_t* es)
{
	GList* list = es->effectList;
	guint val = gtk_range_get_value(range);

	do {
		if( !strcmp(es->currentEffectType, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	} while( list );
	strcpy(es->currentEffectType, ((eachEffect_t*)list->data)->name);
	if( ((eachEffect_t*)list->data)->addrWidth == 2 ){
		// this type of effect need 2-bytes width prameter specification.
		if( !strcmp("Insert1", es->stripName) ){
			sendExc(5, 0x03, 0x00, 0x42, 0x00, val);
		} else if( !strcmp("Insert2", es->stripName) ){
			sendExc(5, 0x03, 0x01, 0x42, 0x00, val);
		} else {
			sendExc(5 , 0x02, 0x01, 0x54, 0x00, val);
		}
	}else{
		if( !strcmp("Insert1", es->stripName) ){
			sendExc(4, 0x03, 0x00, 0x0B, val);
		} else if( !strcmp("Insert2", es->stripName) ){
			sendExc(4, 0x03, 0x01, 0x0B, val);
		} else {
			sendExc(4 , 0x02, 0x01, 0x0B, val);
		}
	}
}
 
void createEffectTypeComboBox(GtkWidget* comboBox, GList* list)
{
	do {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox),\
			((eachEffect_t*)(list->data))->name);
		list = list->next;
	} while( list );
}

void createTargetChnlComboBox(GtkWidget* comboBox)
{
	int itr;
	char** entry;
	int fields;

	for(itr = 0; itr < sizeof(targetChnl)/(sizeof(targetChnl[0])); itr++){
		entry = splitline(targetChnl[itr], ',', &fields);

		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), entry[0]);
	}
}

