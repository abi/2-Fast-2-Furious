#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> //Get rid of this
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#include <pthread.h>

#define MIN(x, y) x > y ? y : x

#define MAX_SIZE 5000000
#define NUM_SCCS 5

int total_nodes = 0;
int total_edges = 0;

int *edges;
int *edges_of_node;

int *threaded_edges[2];
int *threaded_edges_of_node[2];

struct node_data {
  int index;
  int lowlink;
} *node_data;

struct frame {
  int local1;
  int local2;
} stack_frames[MAX_SIZE];

int stack_position = 0;

int *stack;
bool stack_contains[MAX_SIZE];

int largest_sccs[NUM_SCCS] = {0, 0, 0, 0, 0};

//Keep this data in a struct to avoid namespace pollution

struct thread_data {
  char *thread_start[2];
  char *thread_end[2];
  int len[2];
  int num_equal_edges_at_end;
  int num_equal_edges_at_start;
} thread_data;

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

  for (int pos = edges_of_node[v  ];
           pos < edges_of_node[v+1];
           pos++)
    {
      w = edges[pos];

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
          w = edges[pos];

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

void *
thread(int threadid)
{
  int start, end, i = 0;
  register int digit, num;
  int laststart = 0, nodes = 1, j = 0, k = 0;

  if (threadid == 1) thread_data.num_equal_edges_at_start = 0;

  int nodes_seen = 0;

  bool seen2;

  while (thread_data.thread_start[threadid] + j <
         thread_data.thread_end  [threadid])
    {
      k = 0;
      num = 0;

      //extract 2 numbers
      while (k < 2)
        {
EXTRACT_CHAR:
          digit = (int) thread_data.thread_start[threadid][j];
          
          if (digit != 32)
            {
              num = num * 10 + (digit - 48);
              j++;
              goto EXTRACT_CHAR;
            }
          else
            {
              if (k != 0) break;
              start = num;
              j++;
              num = 0;
            }
          k++;
        }

      end = num;
      j += 2;


      if (threadid == 1 && !seen2)
        thread_data.num_equal_edges_at_start++;

      if (start != laststart)
        {
          if (threadid == 1) {
            seen2 = true;
          }

          for (int k=laststart+1;k<start;k++)
            threaded_edges_of_node[threadid][k] = i;

          threaded_edges_of_node[threadid][start] = i;
          nodes_seen = start;
        }
      threaded_edges[threadid][i] = end;

      ++i;
      laststart = start;
    }
  if (threadid == 1)  //TODO: This will cause a bug.
    {
      for (int k=laststart+1;k<=total_nodes+1;k++)
        threaded_edges_of_node[threadid][k] = end;

      thread_data.len[threadid] = total_nodes;
    }
  else
    {
      thread_data.len[threadid] = nodes_seen;
    }
}

/* Spawn 2 threads to load the file */ 

static inline void
load_file (char *input_file)
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
  
  //Over allocating here (not that it really matters).

  //TODO: calloc is for dbg only, set to malloc later.
  //
  //The size should be something approximately like
  //sizeof (int) * (total_edges / 2 + 1)
  threaded_edges[0] = calloc (sizeof (int) * (total_edges + 50), 1); 
  threaded_edges[1] = calloc (sizeof (int) * (total_edges + 50), 1);
  threaded_edges_of_node[0] = calloc (sizeof (int) * (total_nodes + 1), 1);
  threaded_edges_of_node[1] = calloc (sizeof (int) * (total_nodes + 1), 1);

  edges = calloc(sizeof(int) * total_edges, 1);
  edges_of_node = calloc(sizeof(int) * total_edges, 1);

  node_data = calloc (sizeof (node_data) * total_nodes, 1); //Initialize to 0.
  stack = malloc (sizeof (int) * (total_nodes));

  pthread_t thr1, thr2;

  //Force threads to use both cores

  cpu_set_t    cpuset1; 
  CPU_ZERO   (&cpuset1);
  CPU_SET (0, &cpuset1);

  cpu_set_t    cpuset2;
  CPU_ZERO   (&cpuset2);
  CPU_SET (1, &cpuset2);

  pthread_setaffinity_np (thr1, sizeof (cpu_set_t), &cpuset1);
  pthread_setaffinity_np (thr2, sizeof (cpu_set_t), &cpuset2);

  thread_data.thread_start[0] = buf;
  thread_data.thread_start[1] = buf + buf_size / 2;
  thread_data.thread_end  [0] = buf + buf_size / 2;
  thread_data.thread_end  [1] = buf + buf_size;

  //seek back until you find a '\n'
  while (*(--thread_data.thread_start[1]) != '\n')
    ;

  //but don't actually include it
  thread_data.thread_start[1]++;

  thread_data.thread_end[0] = thread_data.thread_start[1];

  //TODO: Maybe we should have some sort of backup plan?
  //This will fail if we can't spawn threads.

  pthread_create (&thr1, NULL, &thread, 0);
  pthread_create (&thr2, NULL, &thread, 1);

  pthread_join (thr1, NULL);
  pthread_join (thr2, NULL);

  data_merge();
}

void
data_merge ()
{
  //TODO: memcpy
  //TODO: store size when threading so we don't have to count it
  //TODO: not sure which is actually faster
  //TODO: could probably change edges_of_node "ptr" with vector asm
  //TODO: just use pointers, not integers in edges_of_node

  //TODO: for threaded_edges_of_node, no need to merge at all. Should be able
  //to write synchronously to the array. This is a big deal!
  //
  //However you will race if the same node shows up on both lists, so we would
  //need to separate the two groups out better.

  int pos = 0;
  int size = 0;
  int len_first_chunk = 0;

  for (int tid=0;tid<2;tid++)
    {
      //Don't know how big each one is so stop at 0 delimiter
      int val;
      for (int i=1; (val = threaded_edges[tid][i]) != 0; i++)
        {
          edges[pos] = val;
          ++pos;
        }
      if (len_first_chunk == 0) len_first_chunk = pos;
    }
  pos = 0;

  //printf("len first chunk %d\n", len_first_chunk);
  int start = 0;
  for (int tid=0;tid<2;tid++)
    {
      for (int i=0; i<thread_data.len[tid]; i++)
        {
          edges_of_node[pos] = threaded_edges_of_node[tid][pos] + start;
          ++pos;
          //printf ("at %d we have %d\n", pos, edges_of_node[pos] );
        }
      start = len_first_chunk + thread_data.num_equal_edges_at_start;
    }
}

static inline void
find_sccs (int sizes[5])
{
  //The data for node i starts at edges[edges_of_node[i]]
  //and ends at edges[edges_of_node[i+1]].

  for (int i=1;i<=total_nodes;i++)
    {
      if (node_data[i].index == 0)
        {
          strongconnect (i);
        }
    }
}
static inline void
debug_print_info()
{
  /*
  for (int i=0;i<total_edges;i++)
    {
      printf("%d\n", edges[i]);
    }

  printf ("---edge connections---\n");

  */
  /*
  for (int tid=0;tid<2;tid++)
    {
      for (int i=0;i<total_edges;i++)
        {
          printf("%d\n", threaded_edges_of_node[tid][i]);
        }
      if (tid==0) printf(" ----------thread 2 --------------\n");
    }

  printf ("---merged---\n");
  */

  for (int i=0;i<total_nodes;i++)
    {
      //printf("%d\n", edges_of_node[i]);
      //printf("the %dth node starts at the %dth line\n", i, 3 +edges_of_node[i]);
    }
}

int
main (int argc, char* argv[])
{
    int sccSizes[5];
    char* inputFile = argv[1];
    char* outputFile = argv[2];
    
    load_file (inputFile);

    debug_print_info();

    //return 0;

    find_sccs (sccSizes);
    
    for (int i=4;i>=1;i--)
    {
      printf ("%d\t", largest_sccs[i]);
    }

    printf ("%d", largest_sccs[0]); //TODO: are we supposed to have a newline?
    return 0;

    // Output the first 5 sccs into a file.
    // int fd = creat (outputFile);
    //    //TODO: This is really bad, probably have to use strcat
    //    char output[11] = {(char) sccSizes[0], '\t', (char) sccSizes[1], '\t',
    //                       (char) sccSizes[2], '\t', (char) sccSizes[3], '\t',
    //                       (char) sccSizes[4], '\n', '\0'};
    //    write (fd, output, sizeof (output));
    //    close (fd);
    
    return 0;
}
