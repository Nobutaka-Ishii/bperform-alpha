#include <bperform.h>
#include <effectStrip.h>
#include <splitline.h>
#include <stdlib.h>

char *targetChnl[] = {"Off,0x7f", "1,0x0", "2,0x1", "3,0x2", "4,0x3", "AD,0x40"};
enum {INSERT, SYSTEM};

void createEffectTypeComboBox(GtkWidget* comboBox, GList* list);
void createTargetChnlComboBox(GtkWidget* comboBox);
void chnlChanged(GtkWidget* combo, effectStrip_t* es);
void effectTypeChanged(GtkWidget* combo, effectStrip_t* es);
void effectScaleChanged(GtkRange* range, effectStrip_t* es);
void editButtonClicked(GtkWidget* button, effectStrip_t* es);
void constructParamStrips( effectStrip_t* es);
gboolean destroyEditWindow(GtkWidget editwindow, effectStrip_t* es);

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

	int stripType;
	int itr;

	// determine the effect group.
	// Insert1, Insert2, Variation : INSERT
	// Chorus, Reverb : SYSTEM
	stripType = INSERT;
	if( (!strcmp("Chorus", stripName)) || (!strcmp("Reverb", stripName)) ){
		stripType = SYSTEM;
	}

	es = (effectStrip_t*)malloc(sizeof(effectStrip_t));
	memset(es, 0, sizeof(effectStrip_t));
	es->currentEffect = (eachEffect_t*)malloc(sizeof(eachEffect_t));
	es->currentEffect->name = (gchar*)malloc(sizeof(gchar) * EFFECT_NAME_LENGTH);


	strcpy(es->stripName, stripName);

	effectBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	typeComboBox = gtk_combo_box_text_new();
	if( stripType == SYSTEM ){
		chnlComboBox = gtk_label_new("Return Level");
		gtk_widget_set_size_request(chnlComboBox, -1, 36); // width, height
		scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
		gtk_range_set_value( GTK_RANGE(scale), 0);
	} else { // INSERT effects
		chnlComboBox = gtk_combo_box_text_new();
		scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);
			// 1 is the hard coded min value for "Through" entry.
		gtk_range_set_value( GTK_RANGE(scale), 1);
		//gtk_widget_set_sensitive( GTK_WIDGET(scale), FALSE);
	}

	label = gtk_label_new(stripName);
	editButton = gtk_button_new_with_label("Edit");

	editWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size( GTK_WINDOW(editWindow), 1, 600);

	es->paramScales = (GtkWidget**)malloc(sizeof(GtkWidget*) * MU100_EFFECT_PARAMS);
	es->paramLabels = (GtkWidget**)malloc(sizeof(GtkWidget*) * MU100_EFFECT_PARAMS);
	es->paramBoxes = (GtkWidget**)malloc(sizeof(GtkWidget*) * MU100_EFFECT_PARAMS);
	for (itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		// parameter name initialization
		es->currentEffect->param[itr].label = \
			(gchar*)malloc(sizeof(gchar) * PARAM_LABEL_NAME_LENGTH);
		strcpy(es->currentEffect->param[itr].label, "null");

	}

	fp = fopen(path, "r");
	es->effectList = prepEffects(fp);
	fclose(fp);


		// edit window's skelton is fixed up till here.
	es->effectBox = effectBox;
	es->chnlComboBox = chnlComboBox;
	es->typeComboBox = typeComboBox;
	es->scale = scale;
	es->label = label;
	es->editButton = editButton;
	es->editWindow = editWindow;
	strcpy(es->currentEffect->name, "Through");
	es->currentTargetChnl = 0x7f; // targetChnl[0] = {"Off,0x7f"} 's entry.
	//es->editWindowBox = editWindowBox;

	// create channel combo box entries

	if( stripType == INSERT ){
		createTargetChnlComboBox(chnlComboBox);
		gtk_combo_box_set_active( GTK_COMBO_BOX(chnlComboBox), 0);
	}

	createEffectTypeComboBox(typeComboBox, es->effectList);
	gtk_combo_box_set_active( GTK_COMBO_BOX(typeComboBox), 0);

	gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(scale), TRUE);

	gtk_box_pack_start( GTK_BOX(effectBox), label, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), chnlComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), typeComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), scale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectBox), editButton, FALSE, TRUE, 0);

	g_signal_connect(G_OBJECT(es->scale), "value-changed", G_CALLBACK(effectScaleChanged), es);
	if( stripType == INSERT ) g_signal_connect(G_OBJECT(es->chnlComboBox),\
		"changed", G_CALLBACK(chnlChanged), es);
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
	int stripType;
	int itr;


	stripType = INSERT;
	if( (!strcmp("Chorus", es->stripName)) || (!strcmp("Reverb", es->stripName)) ){
		stripType = SYSTEM;
	}

	effectName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );

	if( !strcmp(effectName, "Through") || !strcmp(effectName, "Off") ){
		//gtk_widget_set_sensitive( es->editButton, FALSE);
	}

	if( !strcmp(effectName, "Through") ||
		!strcmp(effectName, "Off") ){
		//gtk_widget_set_sensitive( es->editButton, FALSE);
		gtk_range_set_range( GTK_RANGE(es->scale), 1, 2);
		gtk_range_set_value( GTK_RANGE(es->scale), 1);
		gtk_widget_set_sensitive( es->scale, FALSE);
		return;
	}

	//gtk_widget_set_sensitive( es->editButton, TRUE);
	gtk_range_set_range( GTK_RANGE(es->scale), \
			((eachEffect_t*)(list->data))->param[9].rangeMin, \
			((eachEffect_t*)(list->data))->param[9].rangeMax);
	gtk_widget_set_sensitive( es->scale, TRUE);

	while( list ){
		if( !strcmp(effectName, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}
	msb = ((eachEffect_t*)list->data)->msb;
	lsb = ((eachEffect_t*)list->data)->lsb;

	strcpy(es->currentEffect->name, ((eachEffect_t*)(list->data))->name);
	for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		strcpy( es->currentEffect->param[itr].label, \
			((eachEffect_t*)(list->data))->param[itr].label);

		es->currentEffect->param[itr].rangeMin = \
			((eachEffect_t*)(list->data))->param[itr].rangeMin;
		es->currentEffect->param[itr].rangeMax = \
			((eachEffect_t*)(list->data))->param[itr].rangeMax;

	}

	if( stripType == INSERT ){
		if( strcmp("null", ((eachEffect_t*)list->data)->param[9].label) ){
			//gtk_widget_set_sensitive(es->scale, TRUE);
			gtk_range_set_range(GTK_RANGE(es->scale), \
				((eachEffect_t*)list->data)->param[9].rangeMin, \
				((eachEffect_t*)list->data)->param[9].rangeMax);
		} else {
			//gtk_widget_set_sensitive(es->scale, FALSE);
			//gtk_range_set_range(GTK_RANGE(es->scale), 0, 0);
		}
	} else {
			// effect return value range is always between 0 and 0x7f by MU100's hw spec.
			gtk_range_set_range(GTK_RANGE(es->scale), 0, 0x7f);
	}

	if( !strcmp("Insert1", es->stripName) ){
		sendExc(5 , 0x03, 0x00, 0x00, msb, lsb);
	} else if( !strcmp("Insert2", es->stripName) ){
		sendExc(5 , 0x03, 0x01, 0x00, msb, lsb);
	} else if( !strcmp("Variation", es->stripName) ){
		sendExc(5 , 0x02, 0x01, 0x40, msb, lsb);
	} else if( !strcmp("Chorus", es->stripName) ){
		sendExc(5 , 0x02, 0x01, 0x20, msb, lsb);
	} else { // system reverb
		sendExc(5 , 0x02, 0x01, 0x00, msb, lsb);
	}

	g_signal_connect(G_OBJECT(es->editButton), "clicked", G_CALLBACK(editButtonClicked), es);
}

void effectScaleChanged(GtkRange* range, effectStrip_t* es)
{
	GList* list = es->effectList;
	guint val = gtk_range_get_value(range);

	do {
		if( !strcmp(es->currentEffect->name, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	} while( list );
	strcpy(es->currentEffect->name, ((eachEffect_t*)list->data)->name);
	if( ((eachEffect_t*)list->data)->addrWidth == 2 ){
		// this type of effect need 2-bytes width prameter specification.
		if( !strcmp("Insert1", es->stripName) ){
			sendExc(5, 0x03, 0x00, 0x42, 0x00, val);
		} else if( !strcmp("Insert2", es->stripName) ){
			sendExc(5, 0x03, 0x01, 0x42, 0x00, val);
		} else {
			// Variation block used as insertion effect. Always 2-byte width.
			sendExc(5 , 0x02, 0x01, 0x54, 0x00, val);
		}
	}else{
		if( !strcmp("Insert1", es->stripName) ){
			sendExc(4, 0x03, 0x00, 0x0B, val);
		} else if( !strcmp("Insert2", es->stripName) ){
			sendExc(4, 0x03, 0x01, 0x0B, val);
		} else if( !strcmp("Chorus", es->stripName) ){
			sendExc(4 , 0x02, 0x01, 0x2C, val); // system chorus return level
		} else {
			sendExc(4 , 0x02, 0x01, 0x0C, val); // system reverb return level
		}
	}
}
 
void createEffectTypeComboBox(GtkWidget* comboBox, GList* list)
{
	do {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), \
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

void editButtonClicked(GtkWidget* button, effectStrip_t* es)
{
	//gtk_widget_set_sensitive( GTK_WIDGET(es->editButton), FALSE);
	constructParamStrips(es);
	gtk_widget_show_all(es->editWindow);
	g_signal_connect( G_OBJECT(es->editWindow), "delete-event", G_CALLBACK(destroyEditWindow), es);
	//g_signal_handlers_disconnect_by_func(G_OBJECT(es->editButton), G_CALLBACK(editButtonClicked), es);
}

gboolean destroyEditWindow(GtkWidget editwindow, effectStrip_t* es){
	g_print("destroy edit window");
	//gtk_widget_destroy( es->editWindowBox);
	gtk_widget_destroy( GTK_WIDGET(es->paramScales[0]) );
	gtk_widget_destroy( GTK_WIDGET(es->paramLabels[0]) );
	gtk_widget_destroy( GTK_WIDGET(es->paramBoxes[0]) );
/*
	g_signal_connect(G_OBJECT(es->editButton), "clicked", G_CALLBACK(editButtonClicked), es);
	gtk_widget_set_sensitive( GTK_WIDGET(es->editButton), TRUE);
	gtk_widget_hide(es->editWindow);
*/
	return FALSE;
}

void constructParamStrips( effectStrip_t* es )
{
	eachEffect_t* ef = es->currentEffect;
	GtkWidget* paramEditFixButton;
	GtkWidget* paramEditFixStrip;
	GtkWidget* editWindowBox;
	GtkWidget* label;
	GtkWidget* scale;
	GtkWidget* box;
	int itr;

	editWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		if( strcmp(ef->param[itr].label, "null") ) {
			label = gtk_label_new( ef->param[itr].label) ;
			scale = gtk_scale_new_with_range( GTK_ORIENTATION_VERTICAL, \
				ef->param[itr].rangeMin, ef->param[itr].rangeMax, 1);
			box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

			gtk_range_set_inverted( GTK_RANGE(scale), TRUE);
			gtk_scale_set_value_pos( GTK_SCALE(scale), GTK_POS_BOTTOM);
			es->paramLabels[itr] = label;
			es->paramScales[itr] = scale;
			es->paramBoxes[itr] = box;

			gtk_box_pack_start( GTK_BOX(box), label, FALSE, FALSE, 0);
			gtk_box_pack_start( GTK_BOX(box), scale, TRUE, TRUE, 0);
			gtk_box_pack_start( GTK_BOX(editWindowBox), box, FALSE, 0, 0);
		}
	}
	paramEditFixButton = gtk_button_new_with_label("OK");
	paramEditFixStrip = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	es->editWindowBox = editWindowBox;
	es->paramEditFixButton = paramEditFixButton;
	es->paramEditFixStrip = paramEditFixStrip;

	gtk_box_pack_end( GTK_BOX(paramEditFixStrip), paramEditFixButton, FALSE, FALSE, 0);
	gtk_box_pack_end( GTK_BOX(editWindowBox), paramEditFixStrip, FALSE, FALSE, 0);
	gtk_container_add( GTK_CONTAINER(es->editWindow), editWindowBox );
	gtk_widget_show_all(es->editWindow);
}

void paramValChanged(GtkWidget* scale, effectStrip_t* es)
{
	guint val = gtk_range_get_value( GTK_RANGE(scale) );
	int paramNum; // 0 - 15, because MU100 has 16 parameters for each effect.
}


