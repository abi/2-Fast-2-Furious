#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
//#include <types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;


int
getRand(int max)
{
  return (rand() % max) + 1;
}

int
main(int argc, char *argv[])
{

  if (argc != 5 && argc != 4)
    {
      cout << "Usage: ./random nodecount edgecount filename [randomseed]" << endl;
      return 0;
    }

  int rseed; //Explaination in PDF
  if (argc == 4)
  {
    rseed = time (NULL);
  } 
  else 
  {
    sscanf (argv[4], "%d", &rseed);
    srand (rseed);
  }

  freopen(argv[3], "w", stdout); //stdout now writes to file

  int nodes;
  int edges;
  sscanf (argv[1], "%d", &nodes);
  sscanf (argv[2], "%d", &edges);

  vector<set<int> > adjList (nodes + 1, set<int> ());

  int i=0;

  /*
   * This has a pathological edge case when the input is of the form n,
   * n*(n-1); that is, when the user asks it to make a complete (directed) 
   * graph.
   *
   * In a case like that, this will take around O(n^2) to make the graph.
   */

  while (i < edges)
  {
    int start = getRand(nodes);
    int end = getRand(nodes);

    if (start==end) continue; //no self loops

    if (adjList[start].find(end) == adjList[start].end())
    {
      adjList[start].insert(end);
      ++i;
    }
  }


  stringstream ss;

  ss << nodes << endl << edges << endl;

  for (i=0;i<adjList.size();i++)
    {
      set<int>::iterator it;
      for (it  = adjList[i].begin();
           it != adjList[i].end  ();
           it++
          )
        ss << i << " " << *it << " " << endl;
    }
    //printf ("Starting write");
    cout << ss.str();

    //printf ("%s\n", ss.str());
    //printf ("Hi\n");
    //printf ("%d\n", sizeof (ss.str()));
  return 0;
}
