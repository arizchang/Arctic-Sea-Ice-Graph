#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv)
{
  ifstream inputFile("1990/Beaufort_Sea_diffw01y1990+landmask", ios::in | ios::binary);
  float dataIn = 0;
  inputFile.read((char*)&dataIn, 4);
  while(!inputFile.eof())
    {
      cout << dataIn << endl;
      inputFile.read((char*)&dataIn, 4);
    }
  inputFile.close();
  return 0;
}
