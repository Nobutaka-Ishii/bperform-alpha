#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <effectStrip.h>
#include <splitline.h>

#define PARAM_LABEL_NAME_LENGTH 32

GList* prepEffects(FILE* fp)
{
	GList* list;
	effectStrip_t* es;
	eachEffect_t* effectEntry;
	char* line = NULL;
	size_t n = 0;
	int itr;
	int fields; // actually not used, because I know the count of entries after splitting. 
	char** eachEffect;
	char** paramLine;

	while( (getline(&line, &n, fp) ) != -1 ){
		if( (line[0] == '\n') || (line[0] == '#') ) continue;
		line[ strlen(line) - 1] = '\0'; // remove last lf character.
		effectEntry = (eachEffect_t*)malloc(sizeof(eachEffect_t));
		memset(effectEntry, 0, sizeof(eachEffect_t));

		list = g_list_append(list, effectEntry);

		eachEffect = splitline(line, '\t', &fields);
		for(itr=0; itr < 18; itr++){ // effect name, msb:lsb:width field, and 1st to 16th parameter.
			switch(itr){ // seeking all 18 tab-separated fields inside this block.
				case 0:
					effectEntry->name = (gchar*)malloc(sizeof(char) * EFFECT_NAME_LENGTH);
					strcpy(effectEntry->name, eachEffect[0]);
					break;
				case 1:
					paramLine = splitline(eachEffect[1], ':', &fields);
					effectEntry->msb = (gint)strtod( paramLine[0], NULL);
					effectEntry->lsb = (gint)strtod( paramLine[1], NULL);
					effectEntry->addrWidth = (gint)strtod( paramLine[2], NULL);
					break;
				default:
					paramLine = splitline(eachEffect[itr], ':', &fields); 
					effectEntry->param[itr -2].label = (gchar*)malloc(sizeof(gchar)* PARAM_LABEL_NAME_LENGTH);
					if( strcmp(paramLine[0], "null") ){
						paramLine = splitline(eachEffect[itr], ':', &fields);
						strcpy(effectEntry->param[itr-2].label, paramLine[0]);
						effectEntry->param[itr-2].rangeMin = (gint)strtod(paramLine[1],NULL);
						effectEntry->param[itr-2].rangeMax = (gint)strtod(paramLine[2],NULL);
					} else {
						strcpy(effectEntry->param[itr-2].label, "null");
					}
					break;
			}			
		}
	}
	return list;
}

