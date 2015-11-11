#include "file.h"

file_t * create_file() {
	file_t * retour = (file_t*)malloc(sizeof(file_t));
	retour->first = NULL;
	retour->last = NULL;
	retour->size = 0;
	pthread_mutex_init(&retour->mutex, NULL);
	return retour;
}

void push_file(file_t * f, char* str) {
	elt_t * tmp = (elt_t*)malloc(sizeof(elt_t));
	if(tmp) {
		tmp->value = (char*)malloc(sizeof(char)*(strlen(str)+1));
		if((tmp->value)) {
			strcpy(tmp->value, str);
			tmp->next = NULL;
			if(f->size > 0) {
				f->last->next = tmp;
				f->last = f->last->next;
			} else {
				f->first = tmp;
				f->last = tmp;
			}
			(f->size)++;
		} else {
			free(tmp);
			tmp = NULL;
		}
	}
}

char * top_file(file_t * f) {
	char * retour = NULL;
	if(f->size > 0) {
		retour = (char*)malloc(sizeof(char)*(strlen(f->first->value)+1));
		strcpy(retour, f->first->value);
	}
	return retour;
}

void pop_file(file_t * f) {
	elt_t * tmp = f->first;
	if(f->size > 0) {
		f->first = tmp->next;
		(f->size)--;
		free(tmp->value);
		free(tmp);
	}
	if(f->size==0) {
		f->last = NULL;
	}
}

void delete_file(file_t * f) {
	while(f->size != 0)
		pop_file(f);
	free(f);
}

