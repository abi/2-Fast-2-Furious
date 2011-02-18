#define _GNU_SOURCE

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> //Get rid of this
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sched.h>

#include <pthread.h>

#define MIN(x, y) x > y ? y : x
#define MAX_SIZE 5000000
#define NUM_SCCS 5

//TODO: Get rid of specfic stack size

int total_nodes = 0;
int total_edges = 0;

char *inputFile;
char *outputFile;

int *edges;
int *threaded_edges[2];

struct thread_data {
  char *thread_start[2];
  char *thread_end[2];
} thread_data;

struct node_data_str {
  int index;
  int lowlink;
  int num_edges;
  void **edges;
};

struct node_data_str *node_data;

struct frame {
  int local1;
  void *local2;
} stack_frames[MAX_SIZE];

int stack_position = 0;

int *stack;
bool stack_contains[MAX_SIZE];

int largest_sccs[NUM_SCCS] = {0};

static inline void stack_push (int v);
static inline int stack_pop ();

static inline void
stack_push (int v)
{
  *stack = v;
  stack_contains[v] = true;
  stack++;
}

static inline int
stack_pop ()
{
  stack--;

  stack_contains[*stack] = false;
  return *stack;
}

static inline void
insert_into_best (int val)
{
  for (int i=0;i<NUM_SCCS;i++)
    {
      if (val > largest_sccs[i])
        {
          if (i==0)
            {
              largest_sccs[i] = val;
            }
          else
            {
              int temp = largest_sccs[i];
              largest_sccs[i] = val;
              largest_sccs[i-1] = temp;
            }
        }
      else break;
    }
}

//TODO: This is temporary

static inline int
extract_num2 (char **strp, const char *delim)
{
  char *str = *strp;
  char *word = strstr (str, delim);
  if (word == NULL) return NULL;
  int len = word - str;
    
  char dest_bf[len + 1];
  strncpy (dest_bf, str, len);
  dest_bf[len] = '\0';
  *strp = (char *) word + 1; //Skip the delim character; TODO: Delim >1 chars
  return atoi (dest_bf);
}

static inline void
strongconnect (int v)
{
  static int lowest_index = 1;

START:

  node_data[v].index = lowest_index;
  node_data[v].lowlink = lowest_index;
  lowest_index++;
  stack_push (v);

  int w;

  //TODO: Discontinuities in the data structure could get us in trouble here.

  for (void *pos = node_data[v].edges;
             pos < (char *)node_data[v].edges + sizeof (int) * node_data[v].num_edges;
             pos = (char *)pos + 4)
    {
      w = *(int *)pos;

      if (node_data[w].index == 0)
        {
          stack_frames[stack_position].local1 = v;
          stack_frames[stack_position].local2 = pos;

          v = w; //call this function with (w)

          stack_position++;
          goto START; 

RETURN:
          stack_position--;
          if (stack_position == -1) 
            {
              stack_position = 0;
              return;
            }

          v =   stack_frames[stack_position].local1;
          pos = stack_frames[stack_position].local2;
          w = *(int *)pos;

          node_data[v].lowlink = MIN(node_data[v].lowlink, node_data[w].lowlink);
        }
      else if (stack_contains[w])
        {
          node_data[v].lowlink = MIN(node_data[v].lowlink, node_data[w].index);
        }
    }

  if (node_data[v].lowlink == node_data[v].index)
  {
    int count = 1;
    while (stack_pop () != v)
      {
        ++count;
      }

    insert_into_best (count);
  }

  goto RETURN;
}

static inline void
read2 (char **buf, int *start, int *end)
{
  int k = 0;
  int num = 0;
  int digit = 0;

  //extract 2 numbers
  while (k < 2)
    {
EXTRACT_CHAR:
      digit = (int) **buf;
      
      if (digit != 32)
        {
          num = num * 10 + (digit - 48);
          (char)(*buf)++;
          goto EXTRACT_CHAR;
        }
      else
        {
            if (k != 0) break;
            *start = num;
            (char)(*buf)++;
            num = 0;
        }
      k++;
    }
    
  *end = num;
  *buf += 2;
}

void *
thread (int tid)
{
  int start, end, i = 0;
  int laststart = -1, nodes = 1, j = 0, k = 0;

  while (thread_data.thread_start[tid] < thread_data.thread_end[tid])
    {
      read2 (&thread_data.thread_start[tid], &start, &end); //TODO manually inline

      if (tid == 1 && laststart == -1)
        {
          laststart = start - 1; //TODO: is this safe?
        }

      for (int k=laststart+1;k<=start;k++) 
        {
          node_data[k].edges = &threaded_edges[tid][i];
        }

      node_data[start].num_edges++;
      threaded_edges[tid][i] = end;
      laststart = start;
      ++i;
    }

  return NULL;
}

static inline void
loadFile (char *input_file)
{
  struct stat statbuf;

  int fd = open (input_file, O_RDONLY);
  fstat (fd, &statbuf);

  char *buf = malloc (statbuf.st_size);
  char *start_ptr = buf;

  //printf ("Buffer size: %d\n", statbuf.st_size);
  read (fd, buf, statbuf.st_size);

  total_nodes = extract_num2 (&buf, "\n");
  total_edges = extract_num2 (&buf, "\n");

  int buf_size = statbuf.st_size - (buf - start_ptr);

  edges = calloc (sizeof (int) * (total_edges), 1);
  node_data = calloc (sizeof (struct node_data_str) * (total_nodes + 1), 1); //Initialize to 0.
  stack = malloc (sizeof (int) * (total_nodes));

  threaded_edges[0] = calloc (sizeof (int) * total_edges, 1);
  threaded_edges[1] = calloc (sizeof (int) * total_edges, 1);


  pthread_t thr1, thr2;

  //Force threads to use both cores

  thread_data.thread_start[0] = buf;
  thread_data.thread_start[1] = buf + buf_size / 2;
  thread_data.thread_end  [0] = buf + buf_size / 2;
  thread_data.thread_end  [1] = buf + buf_size;

  //seek back until you find a '\n'
  while (*(--thread_data.thread_start[1]) != '\n')
    ;

  //but don't actually include it
  thread_data.thread_start[1]++;

  //now seek forward
  int initialstart, start = -1, end = -1;
  void *oldpos;
  read2(&thread_data.thread_start[1], &start, &end);
  initialstart = start;

  //until we find a good division point
  while(start == initialstart)
    {
      oldpos = thread_data.thread_start[1];
      read2(&thread_data.thread_start[1], &start, &end);
    }

  thread_data.thread_start[1] = oldpos;

  thread_data.thread_end[0] = thread_data.thread_start[1];

  //TODO: Maybe we should have some sort of backup plan?
  //This will fail if we can't spawn threads.

  pthread_create (&thr1, NULL, &thread, 0);
  pthread_create (&thr2, NULL, &thread, 1);

  /*
  pthread_setaffinity_np (thr1, sizeof (cpu_set_t), &cpuset1);
  pthread_setaffinity_np (thr2, sizeof (cpu_set_t), &cpuset2);
  */

  pthread_join (thr1, NULL);
  pthread_join (thr2, NULL);
}

static inline void
findSccs (int sizes[5])
{
  for (int i=1;i<=total_nodes;i++)
  {
    if (node_data[i].index == 0)
    {
      strongconnect (i);
    }
  }
}
static inline void
loadFileAfterSegfault (char *input_file)
{
  freopen (input_file, "r", stdin);

  scanf ("%d\n%d\n", &total_nodes, &total_edges);

  stack_position = 0;

  for (int i=0;i<total_nodes;i++)
    {
      stack_frames[i].local1 = 0;
      stack_frames[i].local2 = 0;
    }

  edges = calloc (sizeof (int) * (total_edges), 1);
  node_data = calloc (sizeof (struct node_data_str) * (total_nodes + 1), 1); //Initialize to 0.
  stack = calloc (sizeof (int) * (total_nodes), 1);

  int start, end, i;
  int laststart = -1, nodes = 1, j = 0, k = 0;

  for (i = 0; i < total_edges; i++)
    {
      scanf("%d %d ", &start, &end);

      for (int k=laststart+1;k<=start;k++) 
        {
          node_data[k].edges = &edges[i];
        }

      node_data[start].num_edges++;
      edges[i] = end;
      laststart = start;
    }

  for (int k=laststart+1;k<=total_nodes+1;k++)
    node_data[k].edges = &edges[i];
}


void segfault_handler(int u)
{
  printf("Greetings!\n\n");
  printf("You are seeing this message because your input was not of the correct format.\n");
  printf("Maybe there was an extra space, or not enough spaces, or something else like that.\n");

  printf("\n\n");

  printf("We expect the format to be as provided by the samples. That is:\n\nnodes\nedges\nstart end \n");
  printf("It's important that there's a space after both start and end!\n");
  printf("This makes us really sad, because we now have to revert to a slower version. Here goes:\n");

  freopen (outputFile, "w", stdout); 
  int sccSizes[5];

  loadFileAfterSegfault (inputFile);

  findSccs (sccSizes);

  for (int i=NUM_SCCS-1;i>=1;i--)
    {
      printf ("%d\t", largest_sccs[i]);
    }
  printf ("%d", largest_sccs[0]);

  exit(0);
}

int
main (int argc, char* argv[])
{
    signal (SIGSEGV, segfault_handler);

    int sccSizes[5];
    inputFile = argv[1];
    outputFile = argv[2];
    
    loadFile (inputFile);

    findSccs (sccSizes);
    
    freopen (outputFile, "w", stdout); 
    for (int i=NUM_SCCS-1;i>=1;i--)
      {
        printf ("%d\t", largest_sccs[i]);
      }

    printf ("%d", largest_sccs[0]); //TODO: are we supposed to have a newline?
    return 0;
}

