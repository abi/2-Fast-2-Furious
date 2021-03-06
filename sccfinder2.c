#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN(x, y) x > y ? y : x

int lowestIndex = 1;
int *edges;
int *edgesStartAt;

struct edgeData {
  int index;
  int lowlink;
} *edgeData;

struct frame {
  int local1;
  int local2;
} stackframes[5000000];

int stackposition = 0;

int *stack;
bool stackcontains[5000000];

int bestsofar[5] = {0, 0, 0, 0, 0};

static inline void stack_push(int v);
static inline int stack_pop();

static inline void
stack_push(int v)
{
  *stack = v;
  stackcontains[v] = true;
  stack++;
}

static inline int
stack_pop()
{
  stack--;

  stackcontains[*stack] = false;
  return *stack;
}

static inline void
insert_into_best(int val)
{
  for (int i=0;i<5;i++)
    {
      if (val > bestsofar[i])
        {
          if (i==0)
            {
              bestsofar[i] = val;
            }
          else
            {
              int temp = bestsofar[i];
              bestsofar[i] = val;
              bestsofar[i-1] = temp;
            }
        }
      else break;
    }
}

//#define RECURSIVE

#define ITERATIVE
static inline void
strongconnect(int v)
{
#ifdef ITERATIVE
START:
#endif

  edgeData[v].index = lowestIndex;
  edgeData[v].lowlink = lowestIndex;
  lowestIndex++;
  stack_push(v);

  int w;

  for (int pos = edgesStartAt[v  ];
           pos < edgesStartAt[v+1];
           pos++)
    {
      w = edges[pos];

      if (edgeData[w].index == 0)
        {
          //strongconnect(w);
#ifdef ITERATIVE
          stackframes[stackposition].local1 = v;
          stackframes[stackposition].local2 = pos;

          v = w; //call with (w)

          stackposition++;
          goto START; //HACKY HACKY HACKY HACKY HACKY
#endif

#ifdef RECURSIVE
          strongconnect(w);
#endif 

#ifdef ITERATIVE
RETURN:
          stackposition--;
          if (stackposition == -1) {
            stackposition = 0;
            return;
          }

          v =   stackframes[stackposition].local1;
          pos = stackframes[stackposition].local2;
          w = edges[pos];
#endif

          edgeData[v].lowlink = MIN(edgeData[v].lowlink, edgeData[w].lowlink);
        }
      else if (stackcontains[w])
        {
          edgeData[v].lowlink = MIN(edgeData[v].lowlink, edgeData[w].index);
        }
    }

  if (edgeData[v].lowlink == edgeData[v].index)
  {
    int count = 1;
    while (stack_pop() != v)
      {
        ++count;
      }

    insert_into_best(count);
  }

#ifdef ITERATIVE
  goto RETURN;
#endif
}

static inline void
findSccs (char *input, int sizes[5])
{
	FILE *file = fopen (input, "r");
  //freopen(input, "r", stdin);
  int totalnodes, totaledges;

	//Let's try the setbuf trick
	//Arbitary large number/file size thist time
	char *buffer = malloc (6444768); 
	setbuf (file, buffer);

 	fscanf (file, "%d\n%d\n", &totalnodes, &totaledges);

  edges = malloc (sizeof(int) * totaledges);
  edgesStartAt = malloc (sizeof(int) * (totalnodes + 1));
  edgeData = calloc (sizeof(edgeData) * totalnodes, 1); //Initialize to 0.
  stack = malloc (sizeof(int) * (totalnodes));
  
  int start, end;
  int laststart = 0, nodes = 1;
  int i;

  for (i=0;i<totaledges;i++)
    {
			fscanf (file, "%d\n%d\n", &start, &end);
		 	//scanf("%d\n%d\n", &start, &end);
      
			if (start != laststart)
        {
          for (int k=laststart+1;k<start;k++)
            edgesStartAt[k] = i;

          //printf("%d is now %d\n", start, i); 
          edgesStartAt[start] = i;
          //nodes++;
        }
      edges[i] = end;
      laststart = start;
    }

  for (int k=laststart+1;k<=totalnodes+1;k++)
    edgesStartAt[k] = i;

  for (int i=0;i<totalnodes;i++){
    //printf("%d\n", edgesStartAt[i]);
  }

 /*
  * The data for node i starts at edges[edgesStartAt[i]] and ends at
  * edges[edgesStartAt[i+1]].
  */
  for (int i=1;i<=totalnodes;i++)
  {
    if (edgeData[i].index == 0)
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
    
    findSccs (inputFile, sccSizes);
    
    for (int i=4;i>=0;i--)
    {
      fprintf(stderr, "%d\t", bestsofar[i]);
    }
    if (bestsofar[3] == 0)
      return 0;

    return 1;
    return 0;

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

