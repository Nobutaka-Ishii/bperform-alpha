#include <bperform.h>
#include <effectStrip.h>
#include <splitline.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char *targetChnl[] = {"Off,0x7f", "1,0x0", "2,0x1", "3,0x2", "4,0x3", "AD,0x40"};
enum {INSERT, SYSTEM};

void createEffectTypeComboBox(GtkWidget* comboBox, GList* list);
void createTargetChnlComboBox(GtkWidget* comboBox);
void chnlChanged(GtkWidget* combo, effectStrip_t* es);
void effectTypeChanged(GtkWidget* combo, effectStrip_t* es);
void effectScaleValChanged(GtkRange* range, effectStrip_t* es);
void editButtonClicked(GtkWidget* button, effectStrip_t* es);
void removeParamStripsFromEditWindow( effectStrip_t* es);
void packParamStripsIntoEditWindow( effectStrip_t* es);
gboolean destroyEditWindow(GtkWidget* widget, GdkEvent* event, effectStrip_t* es);
void paramValChanged(GtkWidget* scale, effectStrip_t* es);
void setCurrentEffect( effectStrip_t* es, GList* list, gchar* stripName);

effectStrip_t* effectStripConstr(gchar* stripName, gchar* path)
{
	int fd;
	effectStrip_t* es;
	GtkWidget* effectStripBox;
	GtkWidget* chnlComboBox;
	GtkWidget* effectTypeComboBox;
	GtkWidget* mainParamLabel;
	GtkWidget* scale;
	GtkWidget* label;
	GtkWidget* editButton;
	GtkWidget* editWindow;
	GtkWidget* editWindowBox;
	GtkWidget* paramEditFixButton;
	GtkWidget* paramEditFixStrip;
	int stripType;

	es = (effectStrip_t*)malloc(sizeof(effectStrip_t));

	fd = open(path, O_RDONLY);
	es->effectList = prepEffects(fd);
	close(fd);

	strcpy(es->stripName, stripName);
	// determine the effect group.
	// Insert1, Insert2, Variation : INSERT,  Chorus, Reverb : SYSTEM
	if( (!strcmp("Chorus", stripName)) ){ 
		stripType = SYSTEM;
		setCurrentEffect(es, es->effectList, "Chorus 1");
	} else if(!strcmp("Reverb", stripName)) {
		stripType = SYSTEM;
		setCurrentEffect(es, es->effectList, "Hall 1");
	} else { // INSERT effects
		stripType = INSERT;
		setCurrentEffect(es, es->effectList, "Through");
	}

	label = gtk_label_new(stripName);
	mainParamLabel = gtk_label_new("");
	effectTypeComboBox = gtk_combo_box_text_new();
	createEffectTypeComboBox(effectTypeComboBox, es->effectList);
	gtk_combo_box_set_active( GTK_COMBO_BOX(effectTypeComboBox), 0);
	editButton = gtk_button_new_with_label("Edit");
	if( stripType == SYSTEM ){
		chnlComboBox = gtk_label_new("Return Level");

		gtk_widget_set_size_request(chnlComboBox, -1, 36); // width, height
		scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
		gtk_range_set_value( GTK_RANGE(scale), 127);
	} else { // INSERT effects
		chnlComboBox = gtk_combo_box_text_new();
		createTargetChnlComboBox(chnlComboBox);
		gtk_combo_box_set_active( GTK_COMBO_BOX(chnlComboBox), 0);
		scale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 1, 127, 1);
			// 1 is the hard coded min value for "Through" entry.
		gtk_range_set_value( GTK_RANGE(scale), 1);
	}

	paramEditFixButton = gtk_button_new_with_label("OK");
	paramEditFixStrip = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	editWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	editWindowBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_window_set_default_size(GTK_WINDOW(editWindow), -1, 300);
	effectStripBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

		// On edit window, OK button's strip is always valid.
		// Other parameters' strips depends on.
	gtk_box_pack_end( GTK_BOX(paramEditFixStrip), paramEditFixButton, FALSE, FALSE, 0);
	gtk_box_pack_end( GTK_BOX(editWindowBox), paramEditFixStrip, FALSE, FALSE, 0);
	gtk_container_add( GTK_CONTAINER(editWindow), GTK_WIDGET(editWindowBox) );

		// edit window's skelton is fixed up till here.
	es->effectStripBox = effectStripBox;
	es->mainParamLabel = mainParamLabel;
	es->label = label;
	es->chnlComboBox = chnlComboBox;
	es->effectTypeComboBox = effectTypeComboBox;
	es->scale = scale;
	es->editButton = editButton;
	es->paramEditFixButton = paramEditFixButton;
	es->paramEditFixStrip = paramEditFixStrip;
	es->editWindow = editWindow;
	es->editWindowBox = editWindowBox;
	packParamStripsIntoEditWindow(es);
	if( !strcmp(stripName, "Chorus") ){
		strcpy(es->currentEffect.name, "Chorus 1");
	} else if ( !strcmp(stripName, "Reverb") ){
		strcpy(es->currentEffect.name, "Hall 1");
	} else {
		strcpy(es->currentEffect.name, "Through");
	}
	es->currentTargetChnl = 0x7f; // targetChnl[0] = {"Off,0x7f"} 's entry.

	gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(scale), TRUE);

	gtk_box_pack_start( GTK_BOX(effectStripBox), label, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), chnlComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), effectTypeComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), mainParamLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), scale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(effectStripBox), editButton, FALSE, TRUE, 0);

	g_signal_connect(G_OBJECT(es->scale), "value-changed", G_CALLBACK(effectScaleValChanged), es);
	if( stripType == INSERT ) g_signal_connect(G_OBJECT(es->chnlComboBox), "changed", G_CALLBACK(chnlChanged), es);
	g_signal_connect(G_OBJECT(es->effectTypeComboBox), "changed", G_CALLBACK(effectTypeChanged), es);
	g_signal_connect( G_OBJECT(es->editWindow), "delete-event", G_CALLBACK(destroyEditWindow), es);
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

	if( (!strcmp("Chorus", es->stripName)) || (!strcmp("Reverb", es->stripName)) ){
		stripType = SYSTEM;
	} else {
		stripType = INSERT;
	}

	effectName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(combo) );
	setCurrentEffect(es, list, effectName);
	packParamStripsIntoEditWindow(es);

	if( strcmp( es->currentEffect.mainParam, "null") ){
		gtk_label_set_text( GTK_LABEL(es->mainParamLabel), es->currentEffect.mainParam);
	} else {
		gtk_label_set_text( GTK_LABEL(es->mainParamLabel), "");
	}

	// main scale configuration.
	if( stripType == SYSTEM ){
		gtk_range_set_range( GTK_RANGE(es->scale), 0, 0x7f);
	} else {
		// mainParam's parameter range must be set

		for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
			// search for the parameter number to pick up 
			if( !strcmp(es->currentEffect.mainParam,
				es->currentEffect.param[itr].label) )break;
		}

		gtk_range_set_range( GTK_RANGE(es->scale), es->currentEffect.param[itr].rangeMin,
				es->currentEffect.param[itr].rangeMax);
	}

	msb = es->currentEffect.msb;
	lsb = es->currentEffect.lsb;
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

void effectScaleValChanged(GtkRange* range, effectStrip_t* es)
{
	GList* list = es->effectList;
	guint val = gtk_range_get_value(range);

	do {
		if( !strcmp(es->currentEffect.name, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	} while( list );
	//strcpy(es->currentEffect.name, ((eachEffect_t*)list->data)->name);
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
	gtk_widget_set_sensitive( GTK_WIDGET(es->editButton), FALSE);
	gtk_widget_show_all(es->editWindow);
}

gboolean destroyEditWindow(GtkWidget* widget, GdkEvent* event, effectStrip_t* es){
	gtk_widget_set_sensitive( GTK_WIDGET(es->editButton), TRUE);
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
	int column = 0;

	for (itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		if( strcmp(ef->param[itr].label, "null") ) {
			GtkWidget* blankLabel = gtk_label_new("");
			eachParamStrip_t* eps = (eachParamStrip_t*)malloc(sizeof(eachParamStrip_t));
			memset(eps, 0, sizeof(eachParamStrip_t));

			es->paramStrips[itr] = eps;
			eps->paramScale = gtk_scale_new(GTK_ORIENTATION_VERTICAL, NULL);
			eps->paramLabel = gtk_label_new("");
			eps->paramBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
			gtk_label_set_text(GTK_LABEL(eps->paramLabel),ef->param[itr].label);
			gtk_range_set_round_digits(GTK_RANGE(eps->paramScale), 0);
			gtk_range_set_range(GTK_RANGE(eps->paramScale),
				ef->param[itr].rangeMin, (gint)ef->param[itr].rangeMax);
			gtk_range_set_value(GTK_RANGE(eps->paramScale),
				(gint)(ef->param[itr].rangeMin + ef->param[itr].rangeMax)/2 );
	
			gtk_range_set_inverted(GTK_RANGE(eps->paramScale),TRUE);
			gtk_scale_set_value_pos(GTK_SCALE(eps->paramScale),GTK_POS_BOTTOM);
	
			if( column % 2 ){
				gtk_box_pack_start( GTK_BOX(eps->paramBox), blankLabel, FALSE,FALSE, 0);
				gtk_box_pack_start( GTK_BOX(eps->paramBox), eps->paramLabel,FALSE,FALSE, 0);
			}else{
				gtk_box_pack_start( GTK_BOX(eps->paramBox), eps->paramLabel,FALSE,FALSE, 0);
				gtk_box_pack_start( GTK_BOX(eps->paramBox), blankLabel, FALSE,FALSE, 0);
			}
			gtk_box_pack_end( GTK_BOX(eps->paramBox), eps->paramScale, TRUE, TRUE, 0);
			gtk_box_pack_start( GTK_BOX(es->editWindowBox), eps->paramBox, FALSE, 0, 0);

			g_signal_connect( G_OBJECT(eps->paramScale), "value-changed",
				G_CALLBACK(paramValChanged), es);

			column++;
		}
	}
}

void paramValChanged(GtkWidget* scale, effectStrip_t* es)
{
	int itr;
	gint val = gtk_range_get_value( GTK_RANGE(scale) );
	guint valMsb;
	guint valLsb;
	gboolean targetParamNum[MU100_EFFECT_PARAMS];

		// determine non-null parameters
	for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		if( strcmp("null", es->currentEffect.param[itr].label) ) targetParamNum[itr] = TRUE;
		else targetParamNum[itr] = FALSE;
	}

	for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		if( !targetParamNum[itr] ) continue;
		if( scale == es->paramStrips[itr]->paramScale ) break;
	} // parameter strip number turned out.

	if( !strcmp(es->stripName, "Insert1") ){
		if( es->currentEffect.addrWidth == 1){
			sendExc(4, 0x03, 0x00, itr+2, val);
		}else{
			valLsb = val & 0x00ff;
			valMsb = (val >>8) & 0x00ff;
			sendExc(5, 0x03, 0x00, 0x30 + (itr*2), valMsb, valLsb);
		}
	} else if( !strcmp(es->stripName, "Insert2") ){
		if( es->currentEffect.addrWidth == 1){
			sendExc(4, 0x03, 0x01, itr+2, val);
		}else{
			valLsb = val & 0x00ff;
			valMsb = (val >>8) & 0x00ff;
			sendExc(5, 0x03, 0x01, 0x30 + (itr*2), valMsb, valLsb);
		}
	} else if( !strcmp(es->stripName, "Variation") ){
		// variation effects' parameters are always 2-byte width
		valLsb = val & 0x00ff;
		valMsb = (val >>8) & 0x00ff;
		sendExc(5, 0x02, 0x01, 0x42 + (itr*2), valMsb, valLsb);
	} else if( !strcmp(es->stripName, "Chorus") ){
		sendExc(4, 0x02, 0x01, 0x22 + itr, val);
	} else { // if( !strcmp(es->stripName, "Reverb") ){
		sendExc(4, 0x02, 0x01, 0x02 + itr, val);
	}
}

void setCurrentEffect( effectStrip_t* es, GList* list, gchar* effectName)
{
	int itr;

	while( list ){
		if( !strcmp(effectName, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}

	strcpy(es->currentEffect.name, effectName);
	
	es->currentEffect.msb = ((eachEffect_t*)(list->data))->msb;
	es->currentEffect.lsb = ((eachEffect_t*)(list->data))->lsb;
	strcpy( es->currentEffect.mainParam, ((eachEffect_t*)(list->data))->mainParam); 
	for( itr = 0; itr < MU100_EFFECT_PARAMS; itr++){
		strcpy( es->currentEffect.param[itr].label,
			((eachEffect_t*)(list->data))->param[itr].label);
		es->currentEffect.param[itr].rangeMin =
			((eachEffect_t*)(list->data))->param[itr].rangeMin;
		es->currentEffect.param[itr].rangeMax =
			((eachEffect_t*)(list->data))->param[itr].rangeMax;
		es->currentEffect.addrWidth =
			((eachEffect_t*)(list->data))->addrWidth;
	}
}

