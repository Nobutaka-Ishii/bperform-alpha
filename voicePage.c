#include <bperform.h>
#define TONE_NAME_LENGTH 16

voicePage_t* voicePageConstr(void)
{
	voicePage_t* vpp;
	FILE* fp;
	GList* toneEntries;

	GtkWidget* voicePage;
	GtkWidget* pageContents;
	GtkWidget* pageLeft;
	GtkWidget* pageRight;
	GtkWidget* portaCheckBox;
	GtkWidget* portaTimeScale;
	GtkWidget* monoCheckBox;
	GtkWidget* volBox;
	GtkWidget* volLabel;
	GtkWidget* volScale;
	GtkWidget* panBox;
	GtkWidget* panLabel;
	GtkWidget* panScale;
	GtkWidget* attackBox;
	GtkWidget* attackLabel;
	GtkWidget* attackScale;
	GtkWidget* decayBox;
	GtkWidget* decayLabel;
	GtkWidget* decayScale;
	GtkWidget* releaseBox;
	GtkWidget* releaseLabel;
	GtkWidget* releaseScale;
	GtkWidget* revSendBox;
	GtkWidget* revSendScale;
	GtkWidget* revSendLabel;
	GtkWidget* choSendBox;
	GtkWidget* choSendScale;
	GtkWidget* choSendLabel;
	GtkWidget* prgListComboBox;
	//portaInst_t* portaInst;
	//monoInst_t* monoInst;

	vpp = (voicePage_t*)malloc(sizeof(voicePage_t));
	memset(vpp, 0, sizeof(voicePage_t));

	voicePage = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	pageContents = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	portaCheckBox = gtk_check_button_new_with_label("Portament");
	portaTimeScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	monoCheckBox = gtk_check_button_new_with_label("Mono");
	prgListComboBox = gtk_combo_box_text_new();
	volBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	volLabel = gtk_label_new("V");
	volScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	panBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	panLabel = gtk_label_new("P");
	panScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -63, 63, 1);
	attackBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	attackLabel = gtk_label_new("A");
	attackScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	decayBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	decayLabel = gtk_label_new("D");
	decayScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	releaseBox =  gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	releaseLabel = gtk_label_new("R");
	releaseScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 127, 1);
	pageLeft = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	pageRight = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	//createProgramListComboBox( prgListComboBox, &tones);							 
	revSendBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	revSendLabel = gtk_label_new("Reverb");
	revSendScale= gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
	choSendBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	choSendLabel = gtk_label_new("Chorus");
	choSendScale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);

	if( (fp = fopen("./entries.txt", "r")) ){
		// toneEtnries construction
		toneEntries = createToneEntries(fp);
		fclose(fp);
	}
	
	toneEntries = g_list_first(toneEntries);
	while( toneEntries->next ){
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(prgListComboBox),\
			((eachTone_t*)(toneEntries->data))->name );
		toneEntries = toneEntries->next;
	}

	vpp->voicePage = voicePage;
	vpp->pageContents = pageContents;
	vpp->portaCheckBox = portaCheckBox;
	vpp->portaTimeScale = portaTimeScale;
	vpp->monoCheckBox = monoCheckBox;
	vpp->prgListComboBox = prgListComboBox;
	vpp->volBox = volBox;
	vpp->volLabel = volLabel;
	vpp->volScale = volScale;
	vpp->panLabel = panLabel;
	vpp->panScale = panScale;
	vpp->attackBox = attackBox;
	vpp->attackScale = attackScale;
	vpp->decayBox = decayBox;
	vpp->decayLabel = decayLabel;
	vpp->releaseBox = releaseBox;
	vpp->releaseLabel = releaseLabel;
	vpp->releaseScale = releaseScale;
	vpp->pageLeft = pageLeft;
	vpp->pageRight = pageRight;
	vpp->toneEntries = toneEntries;
	vpp->revSendBox = revSendBox;
	vpp->revSendLabel = revSendLabel;
	vpp->revSendScale = revSendScale;
	vpp->choSendBox = choSendBox;
	vpp->choSendLabel = choSendLabel;
	vpp->choSendScale = choSendScale;

	gtk_scale_set_value_pos(GTK_SCALE(choSendScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(choSendScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(revSendScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(revSendScale), TRUE);

	gtk_box_pack_start( GTK_BOX(pageLeft), prgListComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), volBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), panBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), attackBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), releaseBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), decayBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), monoCheckBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), portaCheckBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), portaTimeScale, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(volBox), volLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(volBox), volScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(panBox), panLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(panBox), panScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(attackBox), attackLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(attackBox), attackScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(decayBox), decayLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(decayBox), decayScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(releaseBox), releaseLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(releaseBox), releaseScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageRight), choSendBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageRight), revSendBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choSendBox), choSendLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choSendBox), choSendScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revSendBox), revSendLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revSendBox), revSendScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageContents), pageLeft, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageContents), pageRight, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(voicePage), pageContents, FALSE, TRUE, 0);


//	signals
	g_signal_connect(volScale, "value-changed",\
		G_CALLBACK(volChanged), NULL);
	g_signal_connect(revSendScale, "value-changed",\
		G_CALLBACK(revSend), NULL);
	g_signal_connect(choSendScale, "value-changed",\
		G_CALLBACK(choSend), NULL);
//  g_signal_connect(G_OBJECT(portaCheckBox), "clicked", G_CALLBACK(portaCheckBoxChecked), &portaInst);
//  g_signal_connect(G_OBJECT(monoCheckBox), "clicked", G_CALLBACK(monoCheckBoxChecked), &monoInst);

		// combobox program select
/*
	g_signal_connect(G_OBJECT(prgListComboBox), "changed", G_CALLBACK(programSelected), tones.toneEntries);
	g_signal_connect(G_OBJECT(portaTimeScale), "value-changed", G_CALLBACK(portaTimeChanged), NULL);
	g_signal_connect(G_OBJECT(attackScale), "value-changed", G_CALLBACK(attackChanged), NULL);
	g_signal_connect(G_OBJECT(decayScale), "value-changed", G_CALLBACK(decayChanged), NULL);
	g_signal_connect(G_OBJECT(releaseScale), "value-changed", G_CALLBACK(releaseChanged), NULL);
*/

	return vpp;
}

GList* createToneEntries(FILE* fp)
{
	GList* toneEntries = NULL;
	char* line;
	char** eachToneLine;
	eachTone_t* eachTonep;
	size_t n = 0; // mandatory for getline, but can be ignored safely.
	int fields; // actually not used, because I know the count of entries after splitting.

	while( ( getline(&line, &n, fp) ) != -1 ){
		if( (line[0] == '\n') || (line[0] == '#') ) continue;
		line [ strlen(line) - 1 ] = '\0';
		eachTonep = (eachTone_t*)malloc(sizeof(eachTone_t));
		memset(eachTonep, 0, sizeof(eachTone_t) );
		eachTonep->name = (char*)malloc(sizeof(TONE_NAME_LENGTH));
		memset(eachTonep->name, 0, (TONE_NAME_LENGTH));
		
		eachToneLine = splitline(line, '\t', &fields);
		strcpy(eachTonep->name, eachToneLine[0]);
		eachTonep->msb = (guint)strtod( eachToneLine[1], NULL);
		eachTonep->lsb = (guint)strtod( eachToneLine[2], NULL);
		eachTonep->pc = (guint)strtod( eachToneLine[3], NULL);

		toneEntries = g_list_append(toneEntries, eachTonep);

	}
	return toneEntries;
}

