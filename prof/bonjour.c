#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void *PthBonjour(void *argument)
{
   printf("Bonjour \n"); 
   pthread_exit(NULL);
   return 0; 
}
int main (int argc, char *argv[])
{
	pthread_t PthId;	
	int rc;
    rc = pthread_create(&PthId, NULL, PthBonjour, NULL);
    assert(0 == rc);
	printf("numero du thread = %d\n", PthId);
/* attente la fin de l'execution du thread */
    rc = pthread_join(PthId, NULL);
    assert(0 == rc);
   	exit(EXIT_SUCCESS);
}
