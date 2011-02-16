#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>

#include <pthread.h>

long long v;
long long w;

//#define THREADED
#define NOT

#define BIGGG 100000000
void
*thread(void *aux)
{
  for (int i=0;i<BIGGG;i++);
  return NULL;
}

void
*thread2(void *aux)
{
  for (int i=0;i<BIGGG;i++);
  return NULL;
}

int main(int argc, char **argv)
{
#ifdef THREADED
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);

  cpu_set_t cpuset1;
  CPU_ZERO(&cpuset1);
  CPU_SET(1, &cpuset1);

  pthread_t thr1, thr2;

  pthread_setaffinity_np(thr1, sizeof(cpu_set_t), &cpuset);
  pthread_setaffinity_np(thr1, sizeof(cpu_set_t), &cpuset1);


  if (pthread_create(&thr1, NULL, &thread, NULL))
  {
    printf ("failed!\n");
    return -1;
  }

  if (pthread_create(&thr2, NULL, &thread2, NULL))
  {
    printf ("failed!\n");
    return -1;
  }

  pthread_join(thr1, NULL);
  pthread_join(thr2, NULL);
#endif

#ifdef NOT
  for (int i=0;i<BIGGG;i++);
  for (int i=0;i<BIGGG;i++);
#endif

  printf("%Ld\n", v); 
  printf("%Ld\n", w); 
  return 0;
}

