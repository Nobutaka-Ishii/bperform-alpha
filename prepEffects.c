#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <effectStrip.h>
#include <splitline.h>
#include <unistd.h>

#define PARAM_LABEL_NAME_LENGTH 32
#define LINEBUFLEN 1024

GList* prepEffects(int fd)
{
	GList* list = NULL;
	eachEffect_t* effectEntry;
	char line[LINEBUFLEN];
	ssize_t n = 0;
	int itr;
	int pos = 0; // head position of the file.
	int fields; // actually not used, because I know the count of entries after splitting. 
	char** eachEffect;
	char** paramFields;

	//memset(line, (char)'\0', LINEBUFLEN);
	while(TRUE){
		if( !(n = read(fd, line, LINEBUFLEN)) ) break;
		for(itr = 0; itr < n ; itr++){
			if(line[itr] == '\n') break;
		}
		pos += itr;
		pos++;
		lseek(fd, pos, SEEK_SET);

		if( (line[0] == '\n') || (line[0] == '#') ) continue;
		line[ itr ] = '\0'; // remove last lf character.
		effectEntry = (eachEffect_t*)malloc(sizeof(eachEffect_t));
		memset(effectEntry, 0, sizeof(eachEffect_t));

		list = g_list_append(list, effectEntry);

		eachEffect = splitline(line, '\t', &fields);
		for(itr=0; itr < 19; itr++){ // effect name, msb:lsb:width field, and 1st to 16th parameter.
			switch(itr){ // seeking all 19 tab-separated fields inside this block.
				case 0:
					strcpy(effectEntry->name, eachEffect[0]);
					break;
				case 1:
					paramFields = splitline(eachEffect[1], ':', &fields);
					effectEntry->msb = (gint)strtod( paramFields[0], NULL);
					effectEntry->lsb = (gint)strtod( paramFields[1], NULL);
					effectEntry->addrWidth = (gint)strtod( paramFields[2], NULL);
					break;
				case 18:
					if( strcmp(eachEffect[itr], "null" )){
						strcpy(effectEntry->mainParam, eachEffect[itr]);
					}else{
						strcpy(effectEntry->mainParam, "null");
					}
					break;
				default:
					paramFields = splitline(eachEffect[itr], ':', &fields); 
					//effectEntry->param[itr -2].label = (gchar*)malloc(sizeof(gchar)* PARAM_LABEL_NAME_LENGTH);
					if( strcmp(paramFields[0], "null") ){
						paramFields = splitline(eachEffect[itr], ':', &fields);
						strcpy(effectEntry->param[itr-2].label, paramFields[0]);
						effectEntry->param[itr-2].rangeMin = (gint)strtod(paramFields[1],NULL);
						effectEntry->param[itr-2].rangeMax = (gint)strtod(paramFields[2],NULL);
					} else {
						strcpy(effectEntry->param[itr-2].label, "null");
					}
					break;
			}			
		}
	}
	return list;
}

