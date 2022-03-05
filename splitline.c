#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define LENGTH 1024

char** splitline(char* original, char delim, int* fields)
{
	int itr = 0;
	int tail = 0;
	char** strs;
	char* src;

	*fields = 0;

	src = (char*)malloc( sizeof(char) * LENGTH);
	strcpy(src, original);

	while( src[itr] ) {
		if (src[itr] == delim){
			(*fields)++;
		}
		itr++;
	}

	strs = (char**)malloc( sizeof(char*) * ((*fields) +1) );

	for( itr = 0; itr < (*fields) +1 ; itr++){
		strs[itr] = (char*)malloc(sizeof(char) * LENGTH);
		memset(strs[itr], 0, LENGTH);
	}

	for (itr = 0; itr < (*fields); itr++){
		while( src[tail] != delim ) tail++;
		strncpy( strs[itr], src, tail);
		tail++;
		src += tail;
		tail = 0;
	}
	if ( itr == (*fields) ) strcpy( strs[itr], src);

	(*fields)++;
	return strs;
}

