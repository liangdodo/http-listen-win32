#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arg.h"

int arg_init(struct arg_t *pa,int argc,char *argv[]){
	pa->argc=argc;
	pa->argv=argv;
	return 0;
}

int arg_is_exists(struct arg_t *pa,const char *name){
	int i;
	
	for(i=0;i<pa->argc;i++){
		if(strcmp(pa->argv[i],name)==0){
			return 1;
		}
	}
	
	return 0;
}

char* arg_get_value(struct arg_t *pa,const char *name){
	int i;
	
	for(i=0;i < pa->argc;i++){
		if((strcmp(pa->argv[i],name)==0) && (i+1 < pa->argc)){
			return pa->argv[i+1];
		}
	}
	
	return NULL;
}
