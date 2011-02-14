/* #define NDEBUG // Set this flag for no debug */
#define MAX_NODE_ID_DIGITS 7

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

//TODO: Bypass C library and do syscalls directly

/* Arbitarily large stack of integers. We don't check 
   for stack overflows. Note: you can only use one stack at
   a time. 
   CONSIDER: Using a node pointer in the stack rather than
   using node->id, then adding it to the base address and
   dereferencing. */

static int *sp = 0;

static inline void
stack_init (size_t size)
{
  sp = malloc (size * sizeof (int));
}

static inline void
stack_push (int el)
{
  *sp = el;
  sp++;
}

static inline int
stack_pop (void)
{
  sp--;
  return *sp;
}

static inline void
stack_test (void)
{
  stack_init (10);
  stack_push (4);
  stack_push (19);
  stack_push (45);  
  assert (stack_pop () == 45);
  stack_push (20);
  assert (stack_pop () == 20);
  assert (stack_pop () == 19);
  assert (stack_pop () == 4);
  stack_push (50);
  assert (stack_pop () == 50);
}


/* Non-standard C Library functions.
   From K&R. TODO: Make GNU style */


//TODO: Will we ever have a negative sign? If not, we can get rid of that part
//from this function.

void itoa(int n, char s[])
{
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s); //TODO: could inline
}
 
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


static inline int
min (int a, int b)
{
  if (a < b) return a;
  return b;
}

/* Globals for the graph */
   
static int cur_index = 1; /* Index that's constantly increasing during DFS */
static void *nodes; /* Pointer to chunk of memory that stores nodes */
static char *edges; /* Pointer to chunk of memory that stores edges */
static int max_scc = 0;

typedef struct node
  {
    int id;
    int index;
    int low_link;
    int stack;
    char *edges;
  } node;
  
typedef struct edge
  {
    int src;
    int dest;
  } edge;
  
static inline node *
get_node (int node_id)
{
  return (node *) nodes + (node_id - 1);
}

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

static inline char *
get_edge (node *n, char *save_ptr, edge *e)
{  
  
  char *c, *ep;
  
  if (save_ptr == NULL)
    { 
      ep = edges;
      char node_id[MAX_NODE_ID_DIGITS];
      node_id[0] = '\n'; //And append a space to the end
      itoa (n->id, &node_id[1]);
      save_ptr = strstr (ep, node_id);
      if (!save_ptr) return NULL;
      save_ptr++; //Skip over the last '\n'
    }
    
  e->src = extract_num2 (&save_ptr, " ");
  if (e->src != n->id) return NULL;   
  e->dest = extract_num2 (&save_ptr, "\n");
  
  return save_ptr; 
}

static inline void
get_edge_test (void)
{
}

/* You have to be careful when you call this
   It moves the global edges pointer
   TODO: Get rid of this
*/

static void
findScc (node *v)
{
  v->index = cur_index;
  v->low_link = cur_index;
  cur_index++;
  v->stack = 1; /* TODO: Should merge this with push */
  stack_push (v->id);
  nodes_explored++;
  ///printf ("Node being explored : %d\n", v->id);
  //printf ("Node being explored : %d\n", v->id);
  
  edge e_struct;
  edge *e = &e_struct;
  node *w;
  char *save_ptr;
  int children = 0;
  
  for (save_ptr = get_edge (v, NULL, e); save_ptr != NULL;
       save_ptr = get_edge (v, save_ptr, e))
    {
      w = get_node (e->dest);
      children++;
      if (w->index == 0)
        {
          findScc (w);
          v->low_link = min (v->low_link, w->low_link);
        }
      else if (w->stack)
        {
          v->low_link = min (v->low_link, w->index);
        }
    }
  
  //printf ("Node %d: \t %d \t %d \t %d\n",
  //        v->id, v->low_link, v->index, children);
  
  if (v->low_link == v->index)
    {
      int scc_len = 0;
      do
        {
          w = get_node (stack_pop ());
          w->stack = 0;
          scc_len++;
        }
      while (w != v);
      
      if (scc_len > max_scc)
        {
          printf ("Max SCC of size: %d\n", scc_len);
          max_scc = scc_len;
        }
    }
}

/**
 * Given an input file (inputFile) and an integer array (out) of size 5, fills
 * the 5 largest SCC sizes into (out) in decreasing order. In the case where
 * there are fewer than 5 components, you should fill in 0 for the remaining
 * values in (out).
 */
static void
findSccs(char* inputFile, int out[5])
{
    int i, n, m, fd;
    
    printf ("Filename : %s\n", inputFile);
    
    //mmap(2) the entire file
    fd = open (inputFile);
    struct stat statbuf;
    fstat (fd, &statbuf);
    
    edges = mmap (NULL, statbuf.st_size + 1, PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (edges == MAP_FAILED)
      {
    	  close(fd);
    	  perror("Error mmapping the file");
    	  exit(EXIT_FAILURE);
      }
    
    //Because '\n' is our delimiters for lines with edge info
    *(edges + statbuf.st_size) = '\n';
    
    n = extract_num2 (&edges, "\n");
    printf ("Number of nodes: %d \n", n);
    m = extract_num2 (&edges, "\n");
    printf ("Number of edges: %d \n", m);
    
    *(--edges) = '\n'; //Because later we look for the sequence '\n'ID
        
    stack_init (n);
    nodes = malloc (sizeof (node) * n);
    
    for (i = 1; i <= n; i++)
      {
        node *nd = get_node (i);
        nd->id = i;
        nd->index = 0;
      }
      
    for (i = 1; i <= n; i++)
      {
        node *nd = get_node (i);
        if (nd->index == 0) /* Need nd->index be 0 initially? */
          findScc (nd);
      }
      
    printf ("MAX : %d\n", max_scc);
      
    out[0] = 65;
    out[1] = 65;
    out[2] = 65;
    out[3] = 65;
    out[4] = 65;
    
    //munmap
    //close
}

/* Test harness function
   If we really want to be fast, we should call this in production */

static void
run_unit_tests (void)
{
  stack_test ();
  get_edge_test ();
}

/*
 * sccfinder should be your main class. If you decide to code in C, you can
 * rename this file to sccfinder.c. We only want your binary to be named
 * sccfinder and we want "make sccfinder" to build sccfinder.
 * Main takes two arguments: 1) input file and 2) output file.
 * You should fill in the findSccs function.
 * Warning: Don't change the part of main that outputs the result of findSccs.
 * It outputs in the correct format.
 */
int
main(int argc, char* argv[])
{
    run_unit_tests (); /* Get rid of this call for opt */
  
    int sccSizes[5];
    char* inputFile = argv[1];
    char* outputFile = argv[2];
    
    findSccs (inputFile, sccSizes);
    
    // Output the first 5 sccs into a file.
    int fd = creat (outputFile);
    //TODO: This is really bad, probably have to use strcat
    char output[11] = {(char) sccSizes[0], '\t', (char) sccSizes[1], '\t',
                       (char) sccSizes[2], '\t', (char) sccSizes[3], '\t',
                       (char) sccSizes[4], '\n', '\0'};
    write (fd, output, sizeof (output));
    close (fd);
    
    return 0;
}
