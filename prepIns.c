#include <string.h>
#include "bperform.h"
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

void prepIns(effects_t* ins)
{
	insEffect* entry;

	// Through ---------------------
	
	entry = (insEffect*)malloc(sizeof(insEffect));
	memset(entry, 0, sizeof(insEffect));

	strcpy(entry->name, "Through");
	entry->msb = 0x40;
	entry->lsb = 0x00;

	strcpy(entry->param10.label, "Dry/Wet");
	entry->param10.rangeMax = 0;
	entry->param10.rangeMin = 0;

	ins->effectList = g_list_append(ins->effectList, entry);

	// Rotary -----------------------

	entry = (insEffect*)malloc(sizeof(insEffect));
	memset(entry, 0, sizeof(insEffect));

	strcpy(entry->name, "Rotary");
	entry->msb = 0x45;
	entry->lsb = 0x00;
	entry->addrWidth = 1;

	strcpy(entry->param1.label, "LFO Frequency");
	entry->param1.rangeMax = 127;
	entry->param1.rangeMin = 0;

	strcpy(entry->param2.label, "LFO Depth");
	entry->param2.rangeMax = 127;
	entry->param2.rangeMin = 0;

	strcpy(entry->param10.label, "Dry/Wet");
	entry->param10.rangeMax = 127;
	entry->param10.rangeMin = 1;

	ins->effectList = g_list_append(ins->effectList, entry);

	// Delay LR -----------------------

	entry = (insEffect*)malloc(sizeof(insEffect));
	memset(entry, 0, sizeof(insEffect));

	strcpy(entry->name, "Delay L,R");
	entry->msb = 6;
	entry->lsb = 0;
	entry->addrWidth = 2;

	strcpy(entry->param1.label, "Lch Delay");
	entry->param1.rangeMax = 7429;
	entry->param1.rangeMin = 1;

	strcpy(entry->param2.label, "Rch Delay");
	entry->param2.rangeMax = 7429;
	entry->param2.rangeMin = 1;

	strcpy(entry->param3.label, "Feedback Delay 1");
	entry->param3.rangeMax = 7429;
	entry->param3.rangeMin = 1;

	strcpy(entry->param4.label, "Feedback Delay 2");
	entry->param4.rangeMax = 7429;
	entry->param4.rangeMin = 1;

	strcpy(entry->param5.label, "Feedback Level");
	entry->param5.rangeMax = 127;
	entry->param5.rangeMin = 1;

	strcpy(entry->param6.label, "High Damp");
	entry->param6.rangeMax = 10;
	entry->param6.rangeMin = 1;

	strcpy(entry->param10.label, "Dry/Wet");
	entry->param10.rangeMax = 127;
	entry->param10.rangeMin = 1;

	ins->effectList = g_list_append(ins->effectList, entry);

	// Amp simulator ----------------

	entry = (insEffect*)malloc(sizeof(insEffect));
	memset(entry, 0, sizeof(insEffect));

	strcpy(entry->name, "Amp Simulator");
	entry->msb = 0x4b;
	entry->lsb = 0;
	entry->addrWidth = 1;

	strcpy(entry->param1.label, "Drive");
	entry->param1.rangeMax = 127;
	entry->param1.rangeMin = 0;

	strcpy(entry->param2.label, "Amp Type");
	entry->param2.rangeMax = 3;
	entry->param2.rangeMin = 0;

	strcpy(entry->param3.label, "LPF");
	entry->param3.rangeMax = 60;
	entry->param3.rangeMin = 34;

	strcpy(entry->param4.label, "Output");
	entry->param4.rangeMax = 127;
	entry->param4.rangeMin = 0;

	strcpy(entry->param10.label, "Dry/Wet");
	entry->param10.rangeMax = 127;
	entry->param10.rangeMin = 1;

	strcpy(entry->param11.label, "Edge");
	entry->param1.rangeMax = 127;
	entry->param1.rangeMin = 0;

	ins->effectList = g_list_append(ins->effectList, entry);

	// Overdrive ----------------

	entry = (insEffect*)malloc(sizeof(insEffect));
	memset(entry, 0, sizeof(insEffect));

	strcpy(entry->name, "Overdrive");
	entry->msb = 0x4a;
	entry->lsb = 0;
	entry->addrWidth = 1;

	strcpy(entry->param1.label, "Drive");
	entry->param1.rangeMax = 127;
	entry->param1.rangeMin = 0;

	strcpy(entry->param4.label, "LPF");
	entry->param4.rangeMax = 60;
	entry->param4.rangeMin = 34;

	strcpy(entry->param5.label, "Output");
	entry->param4.rangeMax = 127;
	entry->param4.rangeMin = 0;

	strcpy(entry->param10.label, "Dry/Wet");
	entry->param10.rangeMax = 127;
	entry->param10.rangeMin = 1;

	strcpy(entry->param11.label, "Edge");
	entry->param1.rangeMax = 127;
	entry->param1.rangeMin = 0;

	ins->effectList = g_list_append(ins->effectList, entry);

	// Distortion ----------------

	entry = (insEffect*)malloc(sizeof(insEffect));
	memset(entry, 0, sizeof(insEffect));

	strcpy(entry->name, "Distortion");
	entry->msb = 0x49;
	entry->lsb = 0;
	entry->addrWidth = 1;

	strcpy(entry->param1.label, "Drive");
	entry->param1.rangeMax = 127;
	entry->param1.rangeMin = 0;

	strcpy(entry->param4.label, "LPF");
	entry->param4.rangeMax = 60;
	entry->param4.rangeMin = 34;

	strcpy(entry->param5.label, "Output");
	entry->param4.rangeMax = 127;
	entry->param4.rangeMin = 0;

	strcpy(entry->param10.label, "Dry/Wet");
	entry->param10.rangeMax = 127;
	entry->param10.rangeMin = 1;

	strcpy(entry->param11.label, "Edge");
	entry->param11.rangeMax = 127;
	entry->param11.rangeMin = 0;

	ins->effectList = g_list_append(ins->effectList, entry);

}

