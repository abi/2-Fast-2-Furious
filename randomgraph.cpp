#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <vector>

using namespace std;


int getRand(int max)
{
  return rand() % max;
}

int main(int argc, char *argv[])
{
  srand (time (NULL));

  if (argc != 4)
    {
      cout << "Usage: ./random nodecount edgecount filename" << endl;
      return 0;
    }

  freopen(argv[3], "w", stdout); //stdout now writes to file

  int nodes;
  int edges;
  sscanf (argv[1], "%d", &nodes);
  sscanf (argv[2], "%d", &edges);

  vector<vector<int> > adjList (nodes, vector<int> (0));

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

    if (find (adjList[start].begin(),
              adjList[start].end(), end) == adjList[start].end())
    {
      adjList[start].push_back(end);
      ++i;
    }
  }

  cout << nodes << endl << edges << endl << endl;

  for (i=0;i<adjList.size();i++)
    for (int j=0;j<adjList[i].size();j++)
      cout << i << " " << adjList[i][j] << endl;

  return 0;
}
