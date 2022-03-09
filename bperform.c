#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <alsa/seq.h>
#include <alsa/seq_event.h>
#include <alsa/seqmid.h>
#include <alsa/seq_midi_event.h>
#include <stdio.h>
#include <bperform.h>
#include <ac1.h>
#include <voicePage.h>
#include <effectStrip.h>

void pgmChange(int pn);
void sendExc(guint length,...);
void sendCc(guint cc, guint val);
void init_synth(void);


snd_seq_t *handle;
int source; // source alsa-client id;
int sport = 0; // app's source MIDI port number
int tport = 0; // target client's midi port number

void initializeSelected(GtkWidget* menu)
{
	// XG reset
	sendExc(4, 0x4c, 0x00, 0x7f, 0x00);
}

int main(int argc, char** argv)
{
	effectStrip_t* ins0strip;
	effectStrip_t* ins1strip;
	ac1_t* ac1p;
	voicePage_t* voicePage0p;
	GList* midiTargets = NULL;

		// variables for alsa connection destination port
	snd_seq_client_info_t *cinfo;
	snd_seq_port_info_t *pinfo;

		// preparation of dst port searching
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_set_client(cinfo, -1);

		// main window instances
	GtkWidget* window;
	GtkWidget* base; // acommodate menubar on top and others on bottom
	GtkWidget* menubar; // occupies top place
	GtkWidget* exceptMenu; // occupies button place
	GtkWidget* ac1menu;

	// voicePage notebook container. This cannot be made into an instance easily.
	GtkWidget* voicePages;

	// page1 entries
	GtkWidget* voicePage1;
	//GtkWidget* page1contents;

	GtkWidget* choReturnBox;
	GtkWidget* choRetScale;
	GtkWidget* choRetLabel;
	GtkWidget* choEditButton;
	GtkWidget* choBlankBox;

	GtkWidget* revReturnBox;
	GtkWidget* revReturnScale;
	GtkWidget* revReturnLabel;
	GtkWidget* revEditButton;
	GtkWidget* revBlankBox;

	GtkWidget* help;
	GtkWidget* helpDownlist;
	GtkWidget* quit;
	GtkWidget* connect;
	GtkWidget* connectDownlist;
	GtkWidget* init;
	GtkWidget* initDownlist;
	GtkWidget* initialize;
	GtkWidget* monauralInit;
	GtkWidget* stereoInit; 

	source = snd_seq_open( &handle, "default", SND_SEQ_OPEN_DUPLEX, 0 );
	if( source < 0 ){
		perror("seq handler open failed");
		return 0;
	}
	snd_seq_set_client_name(handle, "bperform");

	sport = snd_seq_create_simple_port(
		handle, "bperform-port",
		SND_SEQ_PORT_CAP_READ| SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_SOFTWARE
	);

	if ( sport < 0){
		perror("port open failed");
		return 0;
	}

	gtk_init(&argc, &argv);


		// main window and layouts
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	base = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);


		// ac1 window and layouts instance generation
	ac1p = ac1constr();
	voicePage0p = voicePageConstr();

		// insert effect strip construction
	ins0strip = effectStripConstr("Insert1", "./insList.txt");
	ins1strip = effectStripConstr("Insert2", "./insList.txt");

	// create menubar bar
	menubar = gtk_menu_bar_new();

	initDownlist = gtk_menu_new();
	init = gtk_menu_item_new_with_label("Initialize");
	initialize = gtk_menu_item_new_with_label("MU100B init");
	monauralInit = gtk_menu_item_new_with_label("Mono AD init");
	stereoInit = gtk_menu_item_new_with_label("Stereo AD init");
	ac1menu = gtk_menu_item_new_with_label("AC1 config");

	connectDownlist = gtk_menu_new();
	connect = gtk_menu_item_new_with_label("Connect");

	helpDownlist = gtk_menu_new();
	help = gtk_menu_item_new_with_label("Help");
	quit = gtk_menu_item_new_with_label("Quit");

	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), init);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(init), initDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), initialize);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), monauralInit);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), stereoInit);
	gtk_menu_shell_append(GTK_MENU_SHELL(initDownlist), ac1menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), connect);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(connect), connectDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), help);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), helpDownlist);
	gtk_menu_shell_append(GTK_MENU_SHELL(helpDownlist), quit);

	// midi connection target listing up
    while (snd_seq_query_next_client(handle, cinfo) >= 0) {

		midiTarget_t* midiTarget_p;
		char midiTargetName[128]; // used for midi taget name construction
        char strTmp[64]; // used for midi taget name construction

        /* reset query info */
        snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
        snd_seq_port_info_set_port(pinfo, -1);

		while (snd_seq_query_next_port(handle, pinfo) >= 0) {
			midiTarget_p = (midiTarget_t*)malloc( sizeof(midiTarget_t) );
			midiTarget_p->clientId = snd_seq_client_info_get_client(cinfo);
			midiTarget_p->portId = snd_seq_port_info_get_port(pinfo);
			strcpy( midiTarget_p->clientName, snd_seq_port_info_get_name(pinfo) );
			midiTarget_p->checked = 0;

			// construction of entry name
			memset(midiTargetName, 0, 128);
			strcat(midiTargetName, midiTarget_p->clientName);

			memset(strTmp, 0, 64);
			strcat(midiTargetName, ":\t");
			strcat(midiTargetName, strTmp);

			memset(strTmp, 0, 64);
			sprintf(strTmp, "%d", midiTarget_p->clientId);
			strcat(midiTargetName, strTmp);
			strcat(midiTargetName, " / ");

			memset(strTmp, 0, 64);
			sprintf(strTmp, "%d", midiTarget_p->portId);
			strcat(midiTargetName, strTmp);

			midiTargets = g_list_append(midiTargets, midiTarget_p);

			midiTarget_p->checkMenu = gtk_check_menu_item_new_with_label(midiTargetName);
			gtk_menu_shell_append(GTK_MENU_SHELL(connectDownlist), midiTarget_p->checkMenu) ;

		}
	}

	
	// create area other than menubar bar
	exceptMenu = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	//voicePage0 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	voicePage1 = gtk_button_new_with_label("hoge");

	voicePages = gtk_notebook_new();
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voicePage0p->voicePage , GTK_WIDGET(gtk_label_new("Voice0") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voicePage1, GTK_WIDGET(gtk_label_new("Voice1") ) );

/*
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voiceBox, GTK_WIDGET(gtk_label_new("Voice2") ) );
	gtk_notebook_append_page( GTK_NOTEBOOK(voicePages),\
		voiceBox, GTK_WIDGET(gtk_label_new("Voice3") ) );
*/

	choReturnBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); 
	choRetLabel = gtk_label_new("Chorus");
	choRetScale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
	choEditButton = gtk_button_new_with_label("Edit");
	choBlankBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(choBlankBox, -1, 72); // width, height

	revReturnBox =  gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	revReturnLabel = gtk_label_new("Reverb");
	revReturnScale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 127, 1);
	revEditButton = gtk_button_new_with_label("Edit");
	revBlankBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_size_request(revBlankBox, -1, 72); // width, height


    gtk_scale_set_value_pos(GTK_SCALE(choRetScale), GTK_POS_BOTTOM);
    gtk_range_set_inverted(GTK_RANGE(choRetScale), TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(revReturnScale), GTK_POS_BOTTOM);
    gtk_range_set_inverted(GTK_RANGE(revReturnScale), TRUE);


	// callback function settings

		// window events by window manager
	g_signal_connect(G_OBJECT(window), "delete_event",\
		G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",\
		G_CALLBACK(destroy), NULL);
	g_signal_connect(G_OBJECT(ac1menu), "activate", G_CALLBACK( ac1p->ac1menuSelected ), ac1p);


		// menu selection
	g_signal_connect(G_OBJECT(initialize), "activate",\
		G_CALLBACK(initializeSelected), NULL);
	g_signal_connect(G_OBJECT(monauralInit), "activate",\
		G_CALLBACK(monauralInitSelected), NULL);
	g_signal_connect(G_OBJECT(stereoInit), "activate",\
		G_CALLBACK(stereoInitSelected), NULL);
	g_signal_connect(G_OBJECT(quit), "activate",\
		G_CALLBACK(quit_button_pushed), NULL);

		// midi connection button actions
	midiTargets = g_list_first(midiTargets);
	while(midiTargets->next){
		g_signal_connect( \
			G_OBJECT( (GtkWidget*)( ((midiTarget_t*)(midiTargets->data))->checkMenu) ), \
			"activate", G_CALLBACK(targetMidiPortSelected), midiTargets->data);
		midiTargets = midiTargets->next;
	}


	// widgets boxing

	gtk_box_pack_start( GTK_BOX(choReturnBox), choRetLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choReturnBox), choBlankBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choReturnBox), choRetScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(choReturnBox), choEditButton, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(revReturnBox), revReturnLabel, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revReturnBox), revBlankBox, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revReturnBox), revReturnScale, TRUE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(revReturnBox), revEditButton, FALSE, TRUE, 0);

	gtk_box_pack_start( GTK_BOX(exceptMenu), voicePages, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), ins0strip->effectBox, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), ins1strip->effectBox, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), choReturnBox, TRUE, 0, 0);
	gtk_box_pack_start( GTK_BOX(exceptMenu), revReturnBox, TRUE, 0, 0);

	gtk_box_pack_start( GTK_BOX(base), menubar, FALSE, TRUE, 0);
	gtk_box_pack_start( GTK_BOX(base), exceptMenu, TRUE, TRUE, 0);
	gtk_container_add( GTK_CONTAINER(window), base);

	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}

void init_synth(void)
{
	// put variation effect block into insert effect mode.
	sendExc(4, 0x02, 0x01, 0x5b, 64);
}

void sendExc(guint length,...)
{
	snd_seq_event_t ev;
	va_list valist;
	int itr;
	char *data;

	va_start(valist, length);

	data = (char*)malloc( sizeof(guint)*(length + 5) );
	data[0] = 0xf0;
	data[1] = 0x43;
	data[2] = 0x10;
	data[3] = 0x4c;
	data[length + 4]  = 0xf7;

	for( itr=0; itr < length; itr++ ){
		data[4 + itr] = va_arg(valist, guint);
	}
	va_end(valist);

	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_sysex(&ev, 5+length, data);
	snd_seq_ev_set_source(&ev, 0);
	snd_seq_ev_set_direct(&ev);

	ev.source.client = source;
	ev.source.port = sport;
	ev.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
	ev.dest.port = tport;

	snd_seq_event_output(handle, &ev);
	snd_seq_drain_output(handle);
}

void sendCc(guint cc, guint val)
{
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, 0);
    snd_seq_ev_set_direct(&ev);

    ev.source.client = source;
    ev.source.port = sport;
    ev.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
    ev.dest.port = tport;
	ev.type = SND_SEQ_EVENT_CONTROLLER;

	ev.data.control.channel = 0x0;
	ev.data.control.param = cc;
	ev.data.control.value = val;

    snd_seq_event_output(handle, &ev);
    snd_seq_drain_output(handle);
}

void pgmChange(int pn)
{
	snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev, 0);
    snd_seq_ev_set_direct(&ev);

    ev.source.client = source;
    ev.source.port = sport;
    ev.dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
    ev.dest.port = tport;
    ev.type = SND_SEQ_EVENT_PGMCHANGE;

	ev.data.control.channel = 0x0; // MIDI channel
	ev.data.control.param = 0x11; // MIDI Porgarm change ID
	//ev.data.control.value = *((guint*)pn);
	ev.data.control.value = pn;

    snd_seq_event_output(handle, &ev);
    snd_seq_drain_output(handle);
}

void stereoInitSelected(void)
{
	// set AD input as stereo signals : 11 00 00 01
	sendExc(4, 0x11, 0x00, 0x00, 0x01);

	// set AD1 input as line level signal : 10 00 00 01
	sendExc(4, 0x10, 0x00, 0x00, 0x01);

	//set AD2 input as line level signal : 10 01 00 01
	//sendExc(4, 0x10, 0x01, 0x00, 0x01);

	// set variation effect on the path of AD1/2
	sendExc(4, 0x02, 0x01, 0x5b, 64);

	// set send level into variation 100%
	sendExc(4, 0x10, 0x00, 0x14, 127);

	// set send level into directly chorus(effect1) 0%
	sendExc(4, 0x10, 0x00, 0x12, 0);

	// set variation to main path return level 0%
	sendExc(4, 0x02, 0x01, 0x56, 0);

	// AD input master volume 100%
	sendExc(4, 0x10, 0x00, 0x0B, 127);

	// Set effect1(reverb) send level zero.
	sendExc(5, 0x2, 0x1, 0x0, 0x0, 0x0);

	// Set var1effect through
	sendExc(5, 0x2, 0x1, 0x40, 0x40, 0x0);
}

void monauralInitSelected(void)
{
	// set AD input as monaural signal: 11 00 00 00
	sendExc(4, 0x11, 0x00, 0x00, 0x00);

	// set AD1 input as line level signal : 10 00 00 01
	sendExc(4, 0x10, 0x00, 0x00, 0x01);

	//set AD2 input as line level signal : 10 01 00 01
	//sendExc(4, 0x10, 0x01, 0x00, 0x01);

	// set variation effect on the path(insertion mode) of AD1/2
	sendExc(4, 0x02, 0x01, 0x5b, 64);

	// set send level into variation 100%
	sendExc(4, 0x10, 0x00, 0x14, 127);

	// set send level into directly chorus(effect1) 0%
	sendExc(4, 0x10, 0x00, 0x12, 0);

	// set variation to main path return level 0%
	sendExc(4, 0x02, 0x01, 0x56, 0);

	// AD1 input master volume 100% on left channel
	sendExc(4, 0x10, 0x00, 0x0B, 127);

	// AD2 input master volume 0% on right channel
	sendExc(4, 0x10, 0x01, 0x0B, 0);

	// Set effect1(reverb) send level zero.
	sendExc(5, 0x2, 0x1, 0x0, 0x0, 0x0);

	// Set var1effect mode as through
	sendExc(5, 0x2, 0x1, 0x40, 0x40, 0x0);
}

void targetMidiPortSelected( GtkWidget *label, midiTarget_t* midiTarget_p){
	if( midiTarget_p->checked){
		snd_seq_disconnect_to(handle, 0, midiTarget_p->clientId, midiTarget_p->portId);
		midiTarget_p->checked = 0;
	} else {
		snd_seq_connect_to(handle, 0, midiTarget_p->clientId, midiTarget_p->portId);
		midiTarget_p->checked = 1;
	}
}
 
