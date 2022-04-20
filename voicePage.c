#include <bperform.h>
#include <voicePage.h>
#include <splitline.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TONE_NAME_LENGTH 16
#define LINEBUFLEN 1024

const int initialAdVol = 0x00;
const int initialPanVal = 0x40;

voicePage_t* voicePageConstr(gboolean pageType)
{
	voicePage_t* vpp;
	int fd;
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
	GtkWidget* velFixCheckBox;

	vpp = (voicePage_t*)malloc(sizeof(voicePage_t));
	memset(vpp, 0, sizeof(voicePage_t));

	voicePage = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	pageContents = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	portaCheckBox = gtk_check_button_new_with_label("Portament");
	velFixCheckBox = gtk_check_button_new_with_label("Fixed Velocity");
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
	revSendBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	revSendLabel = gtk_label_new("Reverb");
	revSendScale= gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
	choSendBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	choSendLabel = gtk_label_new("Chorus");
	choSendScale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);

	if( (fd = open("./entries.txt", O_RDONLY)) ){
	// toneEtnries construction
		toneEntries = createToneEntries(fd);
		close(fd);
	}
	
	if( pageType == SYNTH ){
		while( toneEntries ) {
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(prgListComboBox),\
				((eachTone_t*)(toneEntries->data))->name );
			if( !(toneEntries->next) ) break;
			toneEntries = toneEntries->next; 
		}
	} else {
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(prgListComboBox), "Mono");
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(prgListComboBox), "Stereo");
	}

	vpp->pageType = pageType;
		// initial value config
	if( pageType == SYNTH ) vpp->vol = 0x7f;
	else  vpp->vol = 0x0;
	vpp->pan = 0x40; // value for being on the center position
	vpp->cho = 0;
	vpp->rev = 0;
	vpp->monoEnabled = FALSE;
	vpp->portaEnabled = FALSE;
	vpp->velFixEnabled = FALSE;
	vpp->portaTime = 0;
	vpp->monoStereo = FALSE;

		// making GUI instances following initial values
	gtk_range_set_value( GTK_RANGE(volScale), vpp->vol);
	gtk_range_set_value( GTK_RANGE(panScale), vpp->pan - 64);
	gtk_range_set_value( GTK_RANGE(revSendScale), vpp->rev);
	gtk_range_set_value( GTK_RANGE(choSendScale), vpp->cho);
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(portaCheckBox), FALSE);
	gtk_range_set_value( GTK_RANGE(portaTimeScale), 0x7f);
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(monoCheckBox), FALSE);

	vpp->voicePage = voicePage;
	vpp->pageContents = pageContents;
	vpp->portaCheckBox = portaCheckBox;
	vpp->portaTimeScale = portaTimeScale;
	vpp->velFixCheckBox = velFixCheckBox;
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

	vpp->programSelected = programSelected;
	vpp->volChanged = volChanged;
	vpp->panChanged = panChanged;
	vpp->attackChanged = attackChanged;
	vpp->decayChanged = decayChanged;
	vpp->releaseChanged = releaseChanged;
	vpp->velFixCheckBoxChecked = velFixCheckBoxChecked;
	vpp->monoCheckBoxChecked = monoCheckBoxChecked;
	vpp->portaCheckBoxChecked = portaCheckBoxChecked;
	vpp->portaTimeChanged = portaTimeChanged;
	vpp->choSend = choSend;
	vpp->revSend = revSend;

	gtk_scale_set_value_pos(GTK_SCALE(choSendScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(choSendScale), TRUE);
	gtk_scale_set_value_pos(GTK_SCALE(revSendScale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(revSendScale), TRUE);

	gtk_box_pack_start( GTK_BOX(pageLeft), prgListComboBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), volBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(pageLeft), panBox, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(volBox), volLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(volBox), volScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(panBox), panLabel, FALSE, FALSE, 0);
	gtk_box_pack_start( GTK_BOX(panBox), panScale, TRUE, TRUE, 0);
	if( pageType == SYNTH ){
		gtk_box_pack_start( GTK_BOX(pageLeft), velFixCheckBox, FALSE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(pageLeft), attackBox, FALSE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(pageLeft), releaseBox, FALSE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(pageLeft), decayBox, FALSE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(pageLeft), monoCheckBox, FALSE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(pageLeft), portaCheckBox, FALSE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(pageLeft), portaTimeScale, FALSE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(attackBox), attackLabel, FALSE, FALSE, 0);
		gtk_box_pack_start( GTK_BOX(attackBox), attackScale, TRUE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(decayBox), decayLabel, FALSE, FALSE, 0);
		gtk_box_pack_start( GTK_BOX(decayBox), decayScale, TRUE, TRUE, 0);
		gtk_box_pack_start( GTK_BOX(releaseBox), releaseLabel, FALSE, FALSE, 0);
		gtk_box_pack_start( GTK_BOX(releaseBox), releaseScale, TRUE, TRUE, 0);
	}
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
	g_signal_connect(G_OBJECT(volScale), "value-changed", G_CALLBACK(vpp->volChanged), vpp);
	g_signal_connect(G_OBJECT(panScale), "value-changed", G_CALLBACK(vpp->panChanged), vpp);
	g_signal_connect(G_OBJECT(revSendScale), "value-changed",\
		G_CALLBACK(vpp->revSend), vpp);
	g_signal_connect(G_OBJECT(choSendScale), "value-changed",\
		G_CALLBACK(vpp->choSend), vpp);

	if( pageType == SYNTH ){
		g_signal_connect(G_OBJECT(attackScale), "value-changed",\
			G_CALLBACK(vpp->attackChanged), vpp);
		g_signal_connect(G_OBJECT(decayScale), "value-changed",\
			G_CALLBACK(vpp->decayChanged), vpp);
		g_signal_connect(G_OBJECT(releaseScale), "value-changed",\
			G_CALLBACK(vpp->releaseChanged), vpp);
		g_signal_connect(G_OBJECT(portaCheckBox), "clicked",\
			G_CALLBACK(vpp->portaCheckBoxChecked), vpp);
		g_signal_connect(G_OBJECT(monoCheckBox), "clicked",\
			G_CALLBACK(vpp->monoCheckBoxChecked), vpp); 
		g_signal_connect(G_OBJECT(velFixCheckBox), "clicked", \
			G_CALLBACK(vpp->velFixCheckBoxChecked), vpp); 
		g_signal_connect(G_OBJECT(portaTimeScale), "value-changed",\
			G_CALLBACK(vpp->portaTimeChanged), vpp);
	}

	//toneEntries = g_list_first(toneEntries);
	// combobox program select
	if ( pageType == SYNTH) {
		g_signal_connect(G_OBJECT(prgListComboBox), "changed",\
			G_CALLBACK(vpp->programSelected), vpp );
	} else {
		gtk_combo_box_set_active( GTK_COMBO_BOX(prgListComboBox), 0); 
		g_signal_connect(G_OBJECT(prgListComboBox), "changed",\
			G_CALLBACK(monoStereoSelected), vpp );
	}

	return vpp;
}

GList* createToneEntries(int fd)
{
	GList* toneEntries = NULL;
	char line[LINEBUFLEN];
	char** eachToneLine;
	eachTone_t* eachTonep;
	ssize_t n = 0;
	int itr;
	int pos = 0;
	int fields; // actually not used, because I know the count of entries after splitting.

	while(TRUE){
		if( !(n = read(fd, line, LINEBUFLEN)) ) break;
		for(itr = 0; itr < n ; itr++){
			if(line[itr] == '\n') break;
		}
		pos += itr;
		pos++;
		lseek(fd, pos, SEEK_SET);
 
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

void programSelected(GtkWidget* pListComboBox, voicePage_t* vpp)
{
	gchar* pName;
	GList* toneEntries = vpp->toneEntries;

	pName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(pListComboBox) );
	toneEntries = g_list_first(toneEntries);

	while( toneEntries ){
		if( !strcmp(pName, ((eachTone_t*)(toneEntries->data))->name ) ) break;
		toneEntries = toneEntries->next;
	}

	sendCc(0, ((eachTone_t*)(toneEntries->data))->msb ); // bank select MSB
	sendCc(32, ((eachTone_t*)(toneEntries->data))->lsb ); // bank select LSB
	pgmChange( ((eachTone_t*)(toneEntries->data))->pc );
}

void portaCheckBoxChecked(GtkWidget* checkbutton, voicePage_t* vpp)
{
	if(!vpp->portaEnabled){
		sendCc(PORTACCNUM, 127);
	}else{
		sendCc(PORTACCNUM, 0);
	}

	vpp->portaEnabled = !vpp->portaEnabled;
}

void velFixCheckBoxChecked(GtkWidget* checkbutton, voicePage_t* vpp)
{
	if(!vpp->velFixEnabled){ // i.e false case, in not-fixed status
		sendExc(4, 0x08, 0x00, 0x0D, 0x7f); // set fixed status
	}else{
		sendExc(4, 0x08, 0x00, 0x0D, 0x40);
	}
	vpp->velFixEnabled = !vpp->velFixEnabled;
}

void portaTimeChanged(GtkRange* scale, voicePage_t* vpp)
{
	guint val = gtk_range_get_value( GTK_RANGE(scale) );
	sendCc(5, val);
	vpp->portaTime = val;
}

void monoCheckBoxChecked(GtkWidget* checkbutton, voicePage_t* vpp)
{
	if(!vpp->monoEnabled){
		sendExc(4, 0x08, 0x00, 0x5, 0x00);
	}else{
		sendExc(4, 0x08, 0x00, 0x5, 0x01);
	}
	vpp->monoEnabled = !vpp->monoEnabled;
}

void volChanged(GtkRange* range, voicePage_t* vpp)
{
	guint val = gtk_range_get_value(range);
	if( vpp->pageType == SYNTH ){
		sendCc(7, val);
	} else {
		sendExc(4, 0x10, 0x00,  0x0B, val);
	}
	vpp->vol = val;
}

void panChanged(GtkRange* range, voicePage_t* vpp)
{
	guint val = gtk_range_get_value(range) + 64;
	if( vpp->pageType == SYNTH ){
		sendCc(10, val);
	} else {
		sendExc(4, 0x10, 0x00,  0x0E, val);
	}
	vpp->pan = val;
}

void revSend(GtkRange* range, voicePage_t* vpp)
{
	guint val = gtk_range_get_value(range);
	if( vpp->pageType == SYNTH ){
		sendCc(91, val);
	} else {
		sendExc(4, 0x10, 0x00,  0x13, val);
	}

	vpp->rev = val;
}

void choSend(GtkRange* range, voicePage_t* vpp)
{
	guint val = gtk_range_get_value(range);
	if( vpp->pageType == SYNTH ){
		sendCc(93, val);
	} else {
		sendExc(4, 0x10, 0x00,  0x12, val);
	}

	vpp->cho = val;
}

void attackChanged(GtkRange* range, voicePage_t* vpp)
{
	guint val = gtk_range_get_value(range);
	sendCc(73, val);
	vpp->attack = val;
}

void decayChanged(GtkRange* range, voicePage_t* vpp)
{
	guint val = gtk_range_get_value(range);
	sendCc(75, val);
	vpp->decay = val;
}

void releaseChanged(GtkRange* range, voicePage_t* vpp)
{
	guint val = gtk_range_get_value(range);
	sendCc(72, val);
	vpp->decay = val;
}

void monoStereoSelected(GtkWidget* pListComboBox, voicePage_t* vpp)
{
	gchar* pName;
	pName = gtk_combo_box_text_get_active_text( GTK_COMBO_BOX_TEXT(pListComboBox) );

	if (!strcmp(pName, "Mono") ){
		vpp->monoStereo = 0;
		monauralInitSelected();
	} else {
		vpp->monoStereo = 1;
		stereoInitSelected();
	}
	vpp->vol = initialAdVol;
	vpp->pan = initialPanVal;

	gtk_range_set_value( GTK_RANGE(vpp->volScale), initialAdVol);
	gtk_range_set_value( GTK_RANGE(vpp->panScale), initialPanVal - 64);
}

void stereoInitSelected(void)
{
	sendExc(4, 0x11, 0x00, 0x00, 0x01); // set AD input as stereo signals : 11 00 00 01
	sendExc(4, 0x10, 0x00, 0x00, 0x01); // set AD1 input as line level signal : 10 00 00 01
	sendExc(4, 0x10, 0x01, 0x00, 0x01); //set AD2 input as line level signal : 10 01 00 01
	sendExc(4, 0x10, 0x00, 0x14, 127); // set send level into variation 100%
	sendExc(4, 0x10, 0x00, 0x12, 0); // set send level into directly chorus(effect1) 0%
	sendExc(4, 0x02, 0x01, 0x56, 0); // set variation to main path return level 0%
	sendExc(4, 0x10, 0x00, 0x0B, initialAdVol); // AD input master volume
}

void monauralInitSelected(void)
{
	sendExc(4, 0x11, 0x00, 0x00, 0x00); // set AD input as monaural signal
	sendExc(4, 0x10, 0x00, 0x00, 0x01); // set AD1 input as line level signal
	sendExc(4, 0x10, 0x01, 0x00, 0x01); //set AD2 input as line level signal
	sendExc(4, 0x10, 0x00, 0x14, 127); // set send level into variation 100%
	sendExc(4, 0x10, 0x00, 0x12, 0); // set send level into directly chorus(effect1) 0%
	sendExc(4, 0x02, 0x01, 0x56, 0); // set variation to main path return level 0%
	sendExc(4, 0x10, 0x00, 0x0B, initialAdVol); // AD1 input master volume on left channel
	sendExc(4, 0x10, 0x01, 0x0B, 0); // AD2 input master volume 0% on right channel
}
