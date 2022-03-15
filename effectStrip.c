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
void removeParamStripsFromEditWindow( effectStrip_t* es);
void packParamStripsIntoEditWindow( effectStrip_t* es);
gboolean destroyEditWindow(effectStrip_t* es);
void paramValChanged(GtkWidget* scale, effectStrip_t* es);

effectStrip_t* effectStripConstr(gchar* stripName, gchar* path)
{
	FILE* fp;
	effectStrip_t* es;
	GtkWidget* effectStripBox;
	GtkWidget* chnlComboBox;
	GtkWidget* effectTypeComboBox;
	GtkWidget* scale;
	GtkWidget* label;
	GtkWidget* editButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;
	GtkWidget* paramEditFixButton;
	GtkWidget* paramEditFixStrip;

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
	for(itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
			// initial effect:through has null.
		strcpy(es->currentEffect.param[itr].label, "null");
	}

	fp = fopen(path, "r");
	es->effectList = prepEffects(fp);
	fclose(fp);

	strcpy(es->stripName, stripName);

	effectStripBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	es->effectStripBox = effectStripBox;


	label = gtk_label_new(stripName);
	effectTypeComboBox = gtk_combo_box_text_new();
	createEffectTypeComboBox(effectTypeComboBox, es->effectList);
	gtk_combo_box_set_active( GTK_COMBO_BOX(effectTypeComboBox), 0);
	editButton = gtk_button_new_with_label("Edit");
	if( stripType == SYSTEM ){
		chnlComboBox = gtk_label_new("Return Level");

		gtk_widget_set_size_request(chnlComboBox, -1, 36); // width, height
		//gtk_widget_set_size_request(effectTypeComboBox, -1, 36); // width, height
		scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
		gtk_range_set_value( GTK_RANGE(scale), 0);
	} else { // INSERT effects
		chnlComboBox = gtk_combo_box_text_new();
		createTargetChnlComboBox(chnlComboBox);
		gtk_combo_box_set_active( GTK_COMBO_BOX(chnlComboBox), 0);
		scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);
			// 1 is the hard coded min value for "Through" entry.
		gtk_range_set_value( GTK_RANGE(scale), 1);
		//gtk_widget_set_sensitive( GTK_WIDGET(scale), FALSE);
	}

	paramEditFixButton = gtk_button_new_with_label("OK");
	paramEditFixStrip = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	editWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	editWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

		// On edit window, OK button's strip is always valid.
		// Other parameters' strips depends on.
	gtk_box_pack_end( GTK_BOX(paramEditFixStrip), paramEditFixButton, FALSE, FALSE, 0);
	gtk_box_pack_end( GTK_BOX(editWindowBox), paramEditFixStrip, FALSE, FALSE, 0);
	gtk_container_add( GTK_CONTAINER(editWindow), GTK_WIDGET(editWindowBox) );

		// edit window's skelton is fixed up till here.
	es->label = label;
	es->chnlComboBox = chnlComboBox;
	es->effectTypeComboBox = effectTypeComboBox;
	es->scale = scale;
	es->editButton = editButton;
	es->paramEditFixButton = paramEditFixButton;
	es->paramEditFixStrip = paramEditFixStrip;
	es->editWindow = editWindow;
	es->editWindowBox = editWindowBox;
	strcpy(es->currentEffect.name, "Through");
	es->currentEffect.msb = 0;
	es->currentEffect.lsb = 0;
	es->currentEffect.addrWidth = 1;
	es->currentTargetChnl = 0x7f; // targetChnl[0] = {"Off,0x7f"} 's entry.

	gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(scale), TRUE);

	gtk_box_pack_start( GTK_BOX(effectStripBox), label, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), chnlComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), effectTypeComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), scale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), editButton, FALSE, TRUE, 0);

	g_signal_connect(G_OBJECT(es->scale), "value-changed", G_CALLBACK(effectScaleChanged), es);
	if( stripType == INSERT ) g_signal_connect(G_OBJECT(es->chnlComboBox), "changed", G_CALLBACK(chnlChanged), es);
	g_signal_connect(G_OBJECT(es->effectTypeComboBox), "changed", G_CALLBACK(effectTypeChanged), es);
	g_signal_connect_swapped( G_OBJECT(es->editWindow), "delete-event", G_CALLBACK(destroyEditWindow), es);
	g_signal_connect(G_OBJECT(es->editButton), "clicked", G_CALLBACK(editButtonClicked), es);
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

	removeParamStripsFromEditWindow(es);

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
		//gtk_widget_set_sensitive( es->scale, FALSE);
		return;
	}

	while( list ){
		if( !strcmp(effectName, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}
	msb = ((eachEffect_t*)list->data)->msb;
	lsb = ((eachEffect_t*)list->data)->lsb;


	strcpy(es->currentEffect.name, ((eachEffect_t*)(list->data))->name);
	for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		strcpy( es->currentEffect.param[itr].label, \
			((eachEffect_t*)(list->data))->param[itr].label);
		es->currentEffect.param[itr].rangeMin = \
			((eachEffect_t*)(list->data))->param[itr].rangeMin;
		es->currentEffect.param[itr].rangeMax = \
			((eachEffect_t*)(list->data))->param[itr].rangeMax;
	}

	if( stripType == INSERT ){
		if( strcmp("null", ((eachEffect_t*)list->data)->param[9].label) ){
			//gtk_widget_set_sensitive(es->scale, TRUE);
			gtk_range_set_round_digits(GTK_RANGE(es->scale), 1);
			gtk_range_set_range(GTK_RANGE(es->scale), \
				((eachEffect_t*)list->data)->param[9].rangeMin, \
				((eachEffect_t*)list->data)->param[9].rangeMax);
			//gtk_widget_set_sensitive( es->scale, TRUE);
		} else {
			//gtk_widget_set_sensitive(es->scale, FALSE);
			//gtk_range_set_range(GTK_RANGE(es->scale), 0, 0);
		}
	} else {
			// effect return value range is always between 0 and 0x7f by MU100's hw spec.
			gtk_range_set_range(GTK_RANGE(es->scale), 0, 0x7f);
	}
	packParamStripsIntoEditWindow(es);

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
}

void effectScaleChanged(GtkRange* range, effectStrip_t* es)
{
	GList* list = es->effectList;
	guint val = gtk_range_get_value(range);

	do {
		if( !strcmp(es->currentEffect.name, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	} while( list );
	strcpy(es->currentEffect.name, ((eachEffect_t*)list->data)->name);
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
	gchar* effectName;
	effectName = gtk_combo_box_text_get_active_text( \
		GTK_COMBO_BOX_TEXT(es->effectTypeComboBox) );
	//gtk_widget_set_sensitive( GTK_WIDGET(es->editButton), FALSE);
	g_print("effect name on the edit button clicking: %s\n", effectName);
	gtk_widget_show_all(es->editWindow);
}

gboolean destroyEditWindow(effectStrip_t* es){
	g_print("edit window destroyed functin\n");
	gtk_widget_hide(es->editWindow);
	return TRUE;
}

void removeParamStripsFromEditWindow( effectStrip_t* es )
{
	eachEffect_t efI = es->currentEffect;
	eachEffect_t* ef = &efI;
	int itr;

	for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		if( strcmp(ef->param[itr].label, "null") ){
			gtk_container_remove( GTK_CONTAINER(es->editWindowBox),
				es->paramStrips[itr]->paramBox);
		}
	}
}

void packParamStripsIntoEditWindow ( effectStrip_t* es )
{
	eachEffect_t efI = es->currentEffect;
	eachEffect_t* ef = &efI;
	int itr;

	for (itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		if( strcmp(ef->param[itr].label, "null") ) {
			eachParamStrip_t* eps = (eachParamStrip_t*)malloc(sizeof(eachParamStrip_t));
			memset(eps, 0, sizeof(eachParamStrip_t));

			es->paramStrips[itr] = eps;
			eps->paramScale = gtk_scale_new(GTK_ORIENTATION_VERTICAL, NULL);
			eps->paramLabel = gtk_label_new("");
			eps->paramBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
			gtk_label_set_text(GTK_LABEL(eps->paramLabel),ef->param[itr].label);
			gtk_range_set_round_digits(GTK_RANGE(eps->paramScale), 0);
			gtk_range_set_range(GTK_RANGE(eps->paramScale),
				ef->param[itr].rangeMin, ef->param[itr].rangeMax);
			gtk_range_set_value(GTK_RANGE(eps->paramScale),
				(ef->param[itr].rangeMin + ef->param[itr].rangeMax)/2 );
	
			gtk_range_set_inverted(GTK_RANGE(eps->paramScale),TRUE);
			gtk_scale_set_value_pos(GTK_SCALE(eps->paramScale),GTK_POS_BOTTOM);
	
			gtk_box_pack_start( GTK_BOX(eps->paramBox), eps->paramLabel, FALSE, FALSE, 0);
			gtk_box_pack_start( GTK_BOX(eps->paramBox), eps->paramScale, TRUE, TRUE, 0);
			gtk_box_pack_start( GTK_BOX(es->editWindowBox), eps->paramBox, FALSE, 0, 0);

			g_signal_connect( G_OBJECT(eps->paramScale), "value-changed",
				G_CALLBACK(paramValChanged), es);
		}
	}
}

void paramValChanged(GtkWidget* scale, effectStrip_t* es)
{
	guint val = gtk_range_get_value( GTK_RANGE(scale) );

		// define which scale is tweaked.
	g_print("param: %d\n", val);
}

void paramScaleTouchFunc(GtkWidget* scale, void* es)
{
}

