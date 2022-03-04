#include <string.h>
#include "bperform.h"
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#define EFFECT_NAME_LENGTH 64
#define PARAM_LABEL_NAME_LENGTH 32

void prepIns(effects_t* ins)
{
	insEffect* effectEntry;
	FILE* fp;
	char* line = NULL;
	size_t n = 0;
	int itr;
	int fields; // actually not used, because I know the count of entries after splitting. 
	char** eachEffect;
	char** param;

	ins->effectList = NULL; 

	fp = fopen("./insList.txt", "r");
	while( (getline(&line, &n, fp) ) != -1 ){
		if( line[0] == '\n' ) continue;
		line[ strlen(line) - 1] = '\0'; // remove last lf character.
		effectEntry = (insEffect*)malloc(sizeof(insEffect));
		memset(effectEntry, 0, sizeof(insEffect));

		ins->effectList = g_list_append(ins->effectList, effectEntry);

		eachEffect = splitline(line, '\t', &fields);
		for(itr=0; itr < 18; itr++){ // effect name, msb:lsb:width field, and 1st to 16th parameter.
			switch(itr){
				case 0:
					effectEntry->name = (gchar*)malloc(sizeof(char) * EFFECT_NAME_LENGTH);
					strcpy(effectEntry->name, eachEffect[0]);
					break;
				case 1:
					param = splitline(eachEffect[1], ':', &fields);
					effectEntry->msb = (gint)strtod( param[0], NULL);
					effectEntry->lsb = (gint)strtod( param[1], NULL);
					effectEntry->addrWidth = (gint)strtod( param[2], NULL);
					break;
				default:
					param = splitline(eachEffect[itr], ':', &fields);
					if( strcmp(param[0], "null") ){
						param = splitline(eachEffect[itr], ':', &fields);
						effectEntry->param[itr].label = (gchar*)malloc(sizeof(gchar)* PARAM_LABEL_NAME_LENGTH);
						strcpy(effectEntry->param[itr].label, param[0]);
						effectEntry->param[itr].rangeMin = (gint)strtod( param[1], NULL);
						effectEntry->param[itr].rangeMax = (gint)strtod( param[2], NULL);
					}
					break;
			}			
		}
	}
}


