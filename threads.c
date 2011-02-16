#include <stdio.h>

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
  v = 0;

  pthread_t thr1, thr2;

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

