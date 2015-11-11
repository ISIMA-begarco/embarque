#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NUM_THREADS  7
int result[8]={0, 0, 0, 0, 0, 0, 0, 0};
pthread_mutex_t my_mutex;
int share=100;
int thread_args[8]={0, 1, 2, 3, 4, 5, 6, 7};
void *TaskCode(void *argument)
{
  int tid;
  tid = *((int *) argument);
  result[tid] = tid;
  pthread_mutex_lock( &my_mutex );
  printf("Exec thread = %d\n", tid);
  share=share - tid;
  pthread_mutex_unlock( &my_mutex );
  pthread_exit( NULL );
  return (0);
}
int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr;
   int rc, i;
   int tmp;
    // initialize the mutex
   tmp = pthread_mutex_init( &my_mutex, NULL );
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
   printf("On commence à creer les threads \n");
   /* creer threads */
   for (i=0; i < NUM_THREADS; i++) {
      thread_args[i] = i;
      pthread_mutex_lock( &my_mutex );
      printf("creer thread no %d \n", i);
      pthread_mutex_unlock( &my_mutex );
      rc = pthread_create(&threads[i], NULL, TaskCode, (void *) &thread_args[i]);
      assert(0 == rc);
   }
   pthread_mutex_lock( &my_mutex );
   printf("Attente la fin des threads \n");
   /* wait for all threads to complete */
   pthread_mutex_unlock( &my_mutex );
   for (i=0; i< NUM_THREADS; i++) {
      rc = pthread_join(threads[i], NULL);
      assert(0 == rc);
   }
   pthread_attr_destroy(&attr);
   for (i=0; i< NUM_THREADS; i++) {
      printf("resultat = %d \n", result[i]);
   }
   printf("share=%d \n", share);
   exit(EXIT_SUCCESS);
}
