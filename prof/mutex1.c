#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define NUM_THREADS  2
int result[NUM_THREADS]={0, 0};
pthread_mutex_t my_mutex;
int share=100;
int thread_args[NUM_THREADS]={0,1};
void *TaskCode1(void *argument)
{
  int tid;
  tid = *((int *)argument);
  result[tid] = tid;
  pthread_mutex_lock( &my_mutex );
  printf("argument du thread0 = %d\n", tid);
  share = share + tid;
  pthread_mutex_unlock( &my_mutex );
  pthread_exit( NULL );
  return 0;
}

void *TaskCode2(void *argument)
{
  int tid;
  tid = *((int *)argument);
  result[tid] = tid;
  pthread_mutex_lock( &my_mutex );
  printf("Argument du thread2 = %d\n", tid);
  share = share + tid;
  pthread_mutex_unlock( &my_mutex );
  pthread_exit( NULL );
  return 0;
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr[NUM_THREADS];
   int rc, i;
   int tmp;
    // initialiser le mutex
   tmp = pthread_mutex_init( &my_mutex, NULL );
   printf("On commence\n");
 	// init attributs
   for (i=0; i < NUM_THREADS; i++) {
      pthread_attr_init(&attr[i]);
      thread_args[i] = i;
   }
   rc = pthread_create(&threads[0], NULL, TaskCode1, (int *) &thread_args[0]);
   assert(0 == rc);
   printf("creer le thread no 1\n");
   rc = pthread_create(&threads[0], NULL, TaskCode2, (int *) &thread_args[1]);
   assert(0 == rc);
   printf("creer le thread no 2\n");
   printf("Attente la fin des threads \n");
   rc = pthread_join(threads[0], NULL);
   rc = pthread_join(threads[1], NULL);
   printf("share=%d \n", share);
   exit(EXIT_SUCCESS);
}
