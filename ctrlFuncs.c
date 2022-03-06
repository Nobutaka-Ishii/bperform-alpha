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
#include "bperform.h"

extern snd_seq_t* handle;
extern int currentVarType;
extern int currentRevType;
extern int source; // source alsa-client id;
extern int sport; // app's source MIDI port number
extern int tport; // target client's midi port number
extern int muteButtonStat; // 1: Mute button is clickable, i.e current unmuted.
extern int varButtonStat; // 1:clickable, current effect is on.
extern int currentVol;
extern int currentRevVal;
extern int dstMaxEntries;
extern struct _midiTarget midiTargets[10];
 
void pgmChange(int pn);
void sendExc(guint length,...);
void sendCc(guint cc, guint val);
void init_synth(void);
void varChanged(GtkRange* range, effects_t* varp);
void ins0changed(GtkRange* range, effects_t* ins0p);
void ins1changed(GtkRange* range, effects_t* ins1p);

void varChanged(GtkRange* range, effects_t* varp)
{
	GList* list = varp->effectList;
	guint val = gtk_range_get_value(range);

	do{
		if( !strcmp(varp->currentInsType, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}while( list );

	sendExc(5, 0x02, 0x01, 0x54, 0x00, val);
}

void ins0changed(GtkRange* range, effects_t* ins0p)
{
	GList* list = ins0p->effectList;
	guint val = gtk_range_get_value(range);

	do{
		if( !strcmp(ins0p->currentInsType, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}while( list );

	if( ((eachEffect_t*)list->data)->addrWidth == 2 ){ 
		// this type of effect need 2-bytes width prameter specification.
		sendExc(5, 0x03, 0x00, 0x42, 0x00, val);
	}else{
		sendExc(4, 0x03, 0x00, 0x0B, val);
	}
}

void ins1changed(GtkRange* range, effects_t* ins1p)
{
	GList* list = ins1p->effectList;
	guint val = gtk_range_get_value(range);

	do{
		if( !strcmp(ins1p->currentInsType, ((eachEffect_t*)(list->data))->name ) ) break;
		list = list->next;
	}while( list );

	if( ((eachEffect_t*)list->data)->addrWidth == 2 ){ 
		// this type of effect need 2-bytes width prameter specification.
		sendExc(5, 0x03, 0x01, 0x42, 0x00, val);
	}else{
		sendExc(4, 0x03, 0x01, 0x0B, val);
	}
}

void volChanged(GtkRange* range)
{
		guint val = gtk_range_get_value(range);
		sendCc(7, val);
}


void revSend(GtkRange* range)
{
		guint val = gtk_range_get_value(range);
		sendCc(91, val);
}

void choSend(GtkRange* range)
{
		guint val = gtk_range_get_value(range);
		sendCc(93, val);
}

void attackChanged(GtkRange* range)
{
		guint val = gtk_range_get_value(range);
		sendCc(73, val);
}

void decayChanged(GtkRange* range)
{
		guint val = gtk_range_get_value(range);
		sendCc(75, val);
}

void releaseChanged(GtkRange* range)
{
		guint val = gtk_range_get_value(range);
		sendCc(72, val);
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

