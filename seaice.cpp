#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

//defn for nodes for adjacency list
struct node
{
  int vertex;
  float* data; //array that will hold data from all the weeks and years
  int color; //0 is white, 1 is gray, 2 is black
  int discoveryTime;
  int finishTime;
  struct node* next;
};

//defn for graph data structure
struct graph
{
  int numVertices;
  int time;
  struct node** adjacencyList; //array of pointers to nodes for adjacency list
};

//functions used
void fillMeans(graph*&, float*&);
float getSxx(node*&, float*&);
float getSxy(node*&, node*&, float*&);
float getR(node*&, node*&, float*&);
void makeGraph(graph*&, float*&);
int getChainSize(node*&);
void dfs(graph*&);
void dfsVisit(graph*&, node*&);

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

  //fills array of means and make graph
  fillMeans(iceGraph, means);
  makeGraph(iceGraph, means);

  int max = 0;
  for(int i = 0; i < 3969; i++)
    {
      //cout << getChainSize(iceGraph->adjacencyList[i]) << endl;
      if(getChainSize(iceGraph->adjacencyList[i]) > max && iceGraph->adjacencyList[i]->data[0] != 168)
	max = getChainSize(iceGraph->adjacencyList[i]);
    }

  int num;  
  for(int i = 0; i <= max; i++)
    {
      num = 0;
      for(int j = 0; j < 3969; j++)
	{
	  if(getChainSize(iceGraph->adjacencyList[j]) == i)
	    num++;
	}
      cout << "Number with degree size " << i << ": " << num << "\t";
      for(int k = 0; k < num/8; k++)
	cout << "*";
      cout << endl;
    }
  return 0;
}

//fills array of means
void fillMeans(graph*& iceGraph, float*& means)
{
  float sum;
  float mean = 0;
  int numIce = 0; //vertices that are not land

  //determines how many vertices are actually ice
  for(int i = 0; i < 3969; i++)
    {
      if(iceGraph->adjacencyList[i]->data[0] != 168)
	numIce++;
    }

  //fills array of means
  for(int i = 0; i < 3969; i++)
    {
      sum = 0;
      if(iceGraph->adjacencyList[i]->data[0] != 168 && iceGraph->adjacencyList[i]->data[0] != 157)
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

//returns Sxx or Syy
float getSxx(node*& point, float*& means)
{
  float sum = 0;
  for(int i = 0; i < 832; i++)
    {
      sum = sum + pow((point->data[i] - means[point->vertex]), 2);
    }
  return sum;
}

//returns Sxy
float getSxy(node*& point1, node*& point2, float*& means)
{
  float sum = 0;
  for(int i = 0; i < 832; i++)
    {
      sum = sum + (point1->data[i] - means[point1->vertex]) * (point2->data[i] - means[point2->vertex]);
    }
  return sum;
}

//returns R
float getR(node*& point1, node*& point2, float*& means)
{
  float Sxx = getSxx(point1, means);
  float Syy = getSxx(point2, means);
  float Sxy = getSxy(point1, point2, means);
  float r = Sxy/(sqrt(Sxx*Syy));
  return r;
}

//constructs graph with adjacency list representation
void makeGraph(graph*& iceGraph, float*& means)
{
  float threshHold = 0.95;
  float R = 0;
  node* current = NULL;
  int count = 0;

  for(int i = 0; i < 3968; i++)
    {
      for(int j = i+1; j < 3969; j++)
	{
	  if(iceGraph->adjacencyList[i]->data[0] != 168 && iceGraph->adjacencyList[j]->data[0] != 168)
	    {
	      R = abs(getR(iceGraph->adjacencyList[i], iceGraph->adjacencyList[j], means));
	      if(R >= threshHold)
		{
		  current = iceGraph->adjacencyList[i];
	      
		  //making copy of node to be inserted
		  node* entry = new node;
		  entry->vertex = iceGraph->adjacencyList[j]->vertex;
		  entry->next = NULL;
		  while(current->next != NULL)
		    current = current->next;
		  current->next = entry;

		  //making a copy of second node to be inserted
		  current = iceGraph->adjacencyList[j];
		  node* entry2 = new node;
		  entry2->vertex = iceGraph->adjacencyList[i]->vertex;
		  entry2->next = NULL;
		  while(current->next !=  NULL)
		    current = current->next;
		  current->next = entry2;
		  
		  count++;
		  //cout << count << endl;
		}
	    }
	}
    }
}

//returns size of adjacency list chain
int getChainSize(node*& point)
{
  int count = 0;
  node* current = point;
  while(current != NULL)
    {
      count++;
      current = current->next;
    }
  return count-1;
}

//function to perform depth first search on graph
void dfs(graph*& iceGraph)
{
  for(int i = 0; i < 3969; i++)
    iceGraph->adjacencyList[i]->color = 0;

  iceGraph->time = 0;
  for(int i = 0; i < 3969; i++)
    {
      if(iceGraph->adjacencyList[i]->color == 0 && iceGraph->adjacencyList[i]->data[0] != 168)
	dfsVisit(iceGraph, iceGraph->adjacencyList[i]);
    }
}

//utility function to go depth first for every node
void dfsVisit(graph*& iceGraph, node*& point)
{
  iceGraph->time++;
  point->discoveryTime = iceGraph->time;
  point->color = 1;
  node* current = point;
  while(current != NULL)
    {
      current = current->next;
      if(current->color == 0)
	dfsVisit(iceGraph, current);
    }
  point->color = 2;
  iceGraph->time++;
  point->finishTime = iceGraph->time;
}
