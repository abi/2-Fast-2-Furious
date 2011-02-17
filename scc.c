#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> //Get rid of this
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MIN(x, y) x > y ? y : x
#define MAX_SIZE 5000000
#define NUM_SCCS 5

//TODO: Get rid of specfic stack size

int total_nodes = 0;

int *edges;
int *edges_of_node;

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

static inline void stack_push(int v);
static inline int stack_pop();

static inline void
stack_push(int v)
{
  *stack = v;
  stack_contains[v] = true;
  stack++;
}

static inline int
stack_pop()
{
  stack--;

  stack_contains[*stack] = false;
  return *stack;
}

static inline void
insert_into_best(int val)
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
strongconnect(int v)
{
  static int lowest_index = 1;

START:

  node_data[v].index = lowest_index;
  node_data[v].lowlink = lowest_index;
  lowest_index++;
  stack_push(v);

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
    while (stack_pop() != v)
      {
        ++count;
      }

    insert_into_best(count);
  }

  goto RETURN;
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
  
  int total_edges; //TODO: Change to n, m
  total_nodes = extract_num2 (&buf, "\n");
  total_edges = extract_num2 (&buf, "\n");
  
  int buf_size = statbuf.st_size - (buf - start_ptr);
  
  edges = malloc (sizeof(int) * total_edges);
  edges_of_node = malloc (sizeof(int) * (total_nodes + 1));
  node_data = calloc (sizeof(node_data) * total_nodes, 1); //Initialize to 0.
  stack = malloc (sizeof(int) * (total_nodes));
  
  int start, end, i;
  register int digit, num;
  int laststart = 0, nodes = 1, j = 0, k = 0;

  for (i = 0; i < total_edges; i++)
    {
      k = 0;
      num = 0;

      //extract 2 numbers
      while (k < 2)
        {
EXTRACT_CHAR:
            digit = (int) buf[j];
            
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

      if (start != laststart)
        {
          for (int k=laststart+1;k<start;k++)
            edges_of_node[k] = i;

          edges_of_node[start] = i;
        }
      edges[i] = end;
      laststart = start;
    }

  for (int k=laststart+1;k<=total_nodes+1;k++)
    edges_of_node[k] = i;

}

static inline void
findSccs (int sizes[5])
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

int
main(int argc, char* argv[])
{
    int sccSizes[5];
    char* inputFile = argv[1];
    char* outputFile = argv[2];
    
    loadFile (inputFile);

    findSccs (sccSizes);
    
    for (int i=4;i>=1;i--)
    {
      printf("%d\t", largest_sccs[i]);
    }
      printf("%d", largest_sccs[0]); //TODO: are we supposed to have a newline?
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
