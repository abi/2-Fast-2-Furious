#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG 1

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
  assert (stack_pop () == 19);
  assert (stack_pop () == 4);
}

/* Test harness function
   If we really want to be fast, we should call this in production */
static void
test (void)
{
  stack_test ();
}


typedef struct node
  {
    int id;
    int index;
    int low_link;
    int stack;
  } node;
  
/**
 * Given an input file (inputFile) and an integer array (out) of size 5, fills
 * the 5 largest SCC sizes into (out) in decreasing order. In the case where
 * there are fewer than 5 components, you should fill in 0 for the remaining
 * values in (out). For example, if there are only 2 components of size 100 and
 * 50, you should fill:
 * out[0] = 100
 * out[1] = 50
 * out[2] = 0
 * out[3] = 0
 * out[4] = 0
 */
void findSccs(char* inputFile, int out[5])
{
    // TODO: Implement this function.
    out[0] = 65;
    out[1] = 65;
    out[2] = 65;
    out[3] = 65;
    out[4] = 65;
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
int main(int argc, char* argv[])
{
    if (DEBUG)
      test ();
  
    int sccSizes[5];
    char* inputFile = argv[1];
    char* outputFile = argv[2];
    //printf ("What up %s \n", inputFile);
    findSccs (inputFile, sccSizes);
    
    
    //printf ("Stack should have worked!\n");
    
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

