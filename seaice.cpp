#include <iostream>
#include <fstream>

using namespace std;

//defn for nodes for adjacency list
struct node
{
  int vertex;
  int* data; //array that will hold data from all the weeks and years
  struct node* next;
};

//defn for graph data structure
struct graph
{
  int numVertices;
  struct node*** adjacencyList; //2D array of pointers to nodes
};

int main(int argc, char** argv)
{
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
  return 0;
}
