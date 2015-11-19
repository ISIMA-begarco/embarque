#ifndef FILE_H
#define FILE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

typedef struct elt {
	char  		* value;
	struct elt 	* next;
} elt_t;

typedef struct file {
	elt_t 		  * first;
	elt_t 		  * last;
	int				size;
	pthread_mutex_t mutex;
} file_t;

file_t * create_file();
void push_file(file_t *, char*);
char * top_file(file_t *);
void pop_file(file_t *);
void delete_file(file_t *);

#endif
