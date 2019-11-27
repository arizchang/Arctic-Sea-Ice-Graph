#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

using namespace std;

//defn for nodes for adjacency list
struct node
{
  int vertex;
  float* data; //array that will hold data from all the weeks and years
  int color; //0 is white, 1 is gray, 2 is black
  int discoveryTime;
  int finishTime;
  float clusteringCo;
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
void fillRList(graph*&, float*&, float*&, float*&);
void fillSxx(graph*&, float*&, float*&);
float getSxx(node*&, float*&);
float getSxy(node*&, node*&, float*&);
float getR(node*&, node*&, float*&, float*&);
void makeGraph(graph*&, float*&, float*&, float);
int getChainSize(node*&);
int  dfs(graph*&);
void dfsVisit(graph*&, node*&, int&);
void deleteList(graph*&);
void setCC(graph*, node*); //clustering coefficient
int getNumEdgesBetweenNeighbors(graph*, node*);

int main(int argc, char** argv)
{
  //make new graph
  graph* iceGraph = new graph;
  iceGraph->numVertices = 3969;
  iceGraph->adjacencyList = new node*[iceGraph->numVertices];
  float* means = new float[3969];
  float* rList = new float[7874496];
  float* sxxList = new float[3969];

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
	  //file = "./code/CSE310_project_subregion/" + to_string(year) + "/Beaufort_Sea_diffw" + weekStr + "y" + to_string(year) + "+landmask";
	  
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
  fillSxx(iceGraph, sxxList, means);
  fillRList(iceGraph, rList, means, sxxList);
  
  //beginning of loop for each threshold
  float threshHold = .95;
  while(threshHold <=.95)
    {
      makeGraph(iceGraph, rList, means, threshHold);

      //finds max degree
      int max = 0;
      for(int i = 0; i < 3969; i++)
	{
	  //cout << getChainSize(iceGraph->adjacencyList[i]) << endl;
	  if(getChainSize(iceGraph->adjacencyList[i]) > max && iceGraph->adjacencyList[i]->data[0] != 168)
	    max = getChainSize(iceGraph->adjacencyList[i]);
	}

      //prints out histogram
      cout << "\nDegree Distribution at threshold " << threshHold << " (Legend: * = 8 data points)" << endl << endl;
      int num;  
      for(int i = 0; i <= max; i++)
	{
	  num = 0;
	  for(int j = 0; j < 3969; j++)
	    {
	      if(iceGraph->adjacencyList[j]->data[0] != 168)
		{
		  if(getChainSize(iceGraph->adjacencyList[j]) == i)
		    num++;
		}
	    }
	  cout << "Number with degree size " << i << ": " << num << "\t";
	  for(int k = 0; k < num/8; k++)
	    cout << "*";
	  cout << endl;
	}
      cout << endl;

      //connected components
      cout << "Connected Components for threshold " << threshHold << endl;
      cout << "Number of connected components: " << dfs(iceGraph) << endl;

      //clustering coefficient
      for(int i = 18; i < 19; i++)
	{
	  if(iceGraph->adjacencyList[i]->data[0] != 168)
	    cout << getNumEdgesBetweenNeighbors(iceGraph, iceGraph->adjacencyList[i]) << " ";
	  if(i%20 == 0)
	    cout << endl;
	}

      //deallocating memory from the adjacency list
      deleteList(iceGraph);

      threshHold += .025;
    }
  return 0;
}

//fills array of means
void fillMeans(graph*& iceGraph, float*& means)
{
  float sum;
  float mean = 0;
  int numIce = 0; //vertices that are not land

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

//fills an array of R values
void fillRList(graph*& iceGraph, float*& rList, float*& means, float*& sxxList)
{
  int count = 0;
  for(int i = 0; i < 3968; i++)
    {
      for(int j = i+1; j < 3969; j++)
	{
	  if(iceGraph->adjacencyList[i]->data[0] != 168 && iceGraph->adjacencyList[j]->data[0] != 168)
	    {
	      rList[count] = abs(getR(iceGraph->adjacencyList[i], iceGraph->adjacencyList[j], means, sxxList));
	      count++;
	    }
	}
    }
}

//fills an array of Sxx/Syy values
void fillSxx(graph*& iceGraph, float*& sxxList, float*& means)
{
  for(int i = 0; i < 3969; i++)
    {
      sxxList[i] = getSxx(iceGraph->adjacencyList[i], means);
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
float getR(node*& point1, node*& point2, float*& means, float*& sxxList)
{
  float Sxx = sxxList[point1->vertex];
  float Syy = sxxList[point2->vertex];
  float Sxy = getSxy(point1, point2, means);
  float r = Sxy/(sqrt(Sxx*Syy));
  return r;
}

//constructs graph with adjacency list representation
void makeGraph(graph*& iceGraph, float*& rList, float*& means, float threshHold)
{
  float R = 0;
  node* current = NULL;
  int count = 0;

  for(int i = 0; i < 3968; i++)
    {
      for(int j = i+1; j < 3969; j++)
	{
	  if(iceGraph->adjacencyList[i]->data[0] != 168 && iceGraph->adjacencyList[j]->data[0] != 168)
	    {
	      R = rList[count];
	      count++;
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

//returns number of connected componenets for graph
int dfs(graph*& iceGraph)
{
  int count = 0;
  int compSize = 0;
  for(int i = 0; i < 3969; i++)
    iceGraph->adjacencyList[i]->color = 0; //initalizing all nodes to white

  iceGraph->time = 0;
  for(int i = 0; i < 3969; i++)
    {
      if(iceGraph->adjacencyList[i]->color == 0 && iceGraph->adjacencyList[i]->data[0] != 168)
	{
	  compSize = 0;
	  dfsVisit(iceGraph, iceGraph->adjacencyList[i], compSize);
	  count++;
	  cout << "Component " << count << " size: " << compSize << "\t";
	  if(count%3 == 0)
	    cout << endl;
	}
    }
  cout << endl;
  return count;
}

//utility function to go depth first for every node
void dfsVisit(graph*& iceGraph, node*& point, int& compSize)
{
  compSize++;
  iceGraph->time++;
  point->discoveryTime = iceGraph->time;
  point->color = 1; //gray node
  node* current = point->next;
  
  while(current != NULL)
    {
      if(iceGraph->adjacencyList[current->vertex]->color == 0)
	dfsVisit(iceGraph, iceGraph->adjacencyList[current->vertex], compSize);
      current = current->next;
    }
 
  point->color = 2; //black node
  iceGraph->time++;
  point->finishTime = iceGraph->time;
}

//deletes linked nodes within adjacency list
void deleteList(graph*& iceGraph)
{
  node* current;
  node* next;
  for(int i = 0; i < 3969; i++)
    {
      current = iceGraph->adjacencyList[i]->next;
      while(current != NULL)
	{
	  next = current->next;
	  delete(current);
	  current = next;
	}
      iceGraph->adjacencyList[i]->next = NULL;
    }
}

//sets the clustering coefficient
void setCC(graph* iceGraph, node* point)
{

}

//returns the number of edges between the neighbors of a vertex
int getNumEdgesBetweenNeighbors(graph* iceGraph, node* point)
{
  vector<int> temp;
  node* current = point->next;
  node* current2;
  int count = 0;

  //fills vector with edges for a vertex
  while(current != NULL)
    {
      temp.push_back(current->vertex);
      //cout << current->vertex << "\t";
      current = current->next;
    }

  //iterating to see neighbors have edges between them
  for(int i = 0; i < temp.size(); i++)
    {
      current2 = iceGraph->adjacencyList[temp[i]]->next;
      while(current2 != NULL)
	{
	  cout << current2->vertex << " and " << temp[i] << "\t";
	  if(current2->vertex == temp[i])
	    {
	      count++;
	      cout << current2->vertex << " and " << temp[i] << "\t";
	    }
	  current2 = current2->next;
	}
    }

  return count/2;
}
