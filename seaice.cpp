#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//functions used


//defn for nodes for adjacency list
struct node
{
  int vertex;
  double* data; //array that will hold data from all the weeks and years
  struct node* next;
};

//defn for graph data structure
struct graph
{
  int numVertices;
  struct node** adjacencyList; //array of pointers to nodes for adjacency list
};

int main(int argc, char** argv)
{
  //make new graph
  graph* iceGraph = new graph;
  iceGraph->numVertices = 3969;
  iceGraph->adjacencyList = new node*[iceGraph->numVertices];

  //declare arrays of data for all vertices
  for(int i = 0; i < iceGraph->numVertices; i++)
    {
      iceGraph->adjacencyList[i] = new node;
      iceGraph->adjacencyList[i]->vertex = i;
      iceGraph->adjacencyList[i]->data = new double[832];
      iceGraph->adjacencyList[i]->next = NULL;
    }

  string file = "";
  int year = 0;
  int week = 0;
  string weekStr = "";

  //iterating through all year folders
  for(int i = 0; i < 16; i++)
    {
      year = 1990 + i;
      //iterating through all weeks
      for(int j = 1; j <= 52; j++)
	{
	  week = j;
	  if(j < 10)
	    weekStr = to_string(0) + to_string(j);
	  else
	    weekStr = to_string(j);
	  file = to_string(year) + "/Beaufort_Sea_diffw" + weekStr + "y" + to_string(year) + "+landmask";
	  cout << file << endl;
	  //opening and reading binary files info arrays of sea ice concentration
	  ifstream inputFile(file, ios::in | ios::binary);
	  double dataIn = 0;
	  int vertex = 0;
	  int time = 0;
	  inputFile.read((char*)&dataIn, 4);
	  while(!inputFile.eof())
	    {
     	      //cout << dataIn << "\t";
	      iceGraph->adjacencyList[vertex]->data[time] = dataIn;
	      inputFile.read((char*)&dataIn, 4);
	      vertex++;
	    }
	  time++;
	  inputFile.close();
	} 
    }

  for(int i = 0; i < 832; i++)
    {
      //cout << iceGraph->adjacencyList[0]->data[i] << endl;
    }

  /*
  ifstream inputFile("1990/Beaufort_Sea_diffw01y1990+landmask", ios::in | ios::binary);
  float dataIn = 0;
  int count = 0;
  inputFile.read((char*)&dataIn, 4);
  while(!inputFile.eof())
    {
      cout << dataIn << " ";
      inputFile.read((char*)&dataIn, 4);
      count++;
      if(count % 63 == 0)
	cout << endl;
    }
  inputFile.close();
  */
  return 0;
}
