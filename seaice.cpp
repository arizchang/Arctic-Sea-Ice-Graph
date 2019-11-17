#include <iostream>
#include <fstream>
#include <string>

using namespace std;

//defn for nodes for adjacency list
struct node
{
  int vertex;
  float* data; //array that will hold data from all the weeks and years
  struct node* next;
};

//defn for graph data structure
struct graph
{
  int numVertices;
  struct node** adjacencyList; //array of pointers to nodes for adjacency list
};

//functions used
void fillMeans(graph*, float*&);

int main(int argc, char** argv)
{
  //make new graph
  graph* iceGraph = new graph;
  iceGraph->numVertices = 3969;
  iceGraph->adjacencyList = new node*[iceGraph->numVertices];
  float* means = new float[3969];

  //declare arrays of data for all vertices
  for(int i = 0; i < iceGraph->numVertices; i++)
    {
      iceGraph->adjacencyList[i] = new node;
      iceGraph->adjacencyList[i]->vertex = i;
      iceGraph->adjacencyList[i]->data = new float[832];
      iceGraph->adjacencyList[i]->next = NULL;
    }

  string file = "";
  int year = 0;
  int week = 0;
  string weekStr = "";
  int vertex;
  int time = 0;

  //iterating through all year folders
  for(int i = 0; i < 16; i++)
    {
      year = 1990 + i;
      //iterating through all weeks
      for(int j = 1; j <= 52; j++)
	{
	  vertex = 0;
	  week = j;
	  if(j < 10)
	    weekStr = to_string(0) + to_string(j);
	  else
	    weekStr = to_string(j);
	  file = to_string(year) + "/Beaufort_Sea_diffw" + weekStr + "y" + to_string(year) + "+landmask";
	  //cout << file << endl;
	  //opening and reading binary files info arrays of sea ice concentration
	  ifstream inputFile(file, ios::in | ios::binary);
	  float dataIn = 0;
	  inputFile.read((char*)&dataIn, 4);
	  while(!inputFile.eof())
	    {
	      iceGraph->adjacencyList[vertex]->data[time] = dataIn;
	      inputFile.read((char*)&dataIn, 4);
	      vertex++;
	    }
	  time++;
	  inputFile.close();
	} 
    }

  //fills array of means
  fillMeans(iceGraph, means);

  for(int i = 0; i < 832; i++)
    cout << means[i] << endl;

  /*  
  ifstream inputFile("2005/Beaufort_Sea_diffw52y2005+landmask", ios::in | ios::binary);
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

//fills array of means
void fillMeans(graph* iceGraph, float*& means)
{
  float sum = 0;
  float mean = 0;
  float numIce = 0; //vertices that are not land

  //determines how many vertices are actually ice
  for(int i = 0; i < 3969; i++)
    {
      if(iceGraph->adjacencyList[i]->data[0] != 168)
	numIce++;
    }

  //fills array of means
  for(int i = 0; i < 3969; i++)
    {
      if(iceGraph->adjacencyList[i]->data[0] != 168)
	{
	  for(int j = 0; j < 832; j++)
	    {
	      sum += iceGraph->adjacencyList[i]->data[j];
	    }
	  mean = sum/832;
	  means[i] = mean;
	}
    } 
}
