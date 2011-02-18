#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <string>
#include <vector>
#include <set>

using namespace std;

int
getRand(int max)
{
  return (rand() % max) + 1;
}

int
main(int argc, char *argv[])
{
  int rseed;
  sscanf (argv[4], "%d", &rseed);
  srand (rseed);

  if (argc != 5)
    {
      cout << "Usage: ./random nodecount edgecount filename randomseed" << endl;
      return 0;
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
  /*
  res += nodes;
  res += "\n";
  res += edges;
  res += "\n";
  */

  for (i=0;i<adjList.size();i++)
    {
      set<int>::iterator it;
      for (it  = adjList[i].begin();
           it != adjList[i].end  ();
           it++
          )
        ss << i << " " << *it << " " << endl;
    }

  cout << ss.str();
  return 0;
}
