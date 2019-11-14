#include <iostream>
#include <fstream>

using namespace std;

int main()
{
  ifstream inputFile("recitationExercise", ios::in | ios::binary);
  float dataIn = 0;
  inputFile.read((char*)&dataIn, 4);
  while(!inputFile.eof())
    {
      cout << "Value is " << dataIn << endl;
      inputFile.read((char*)&dataIn, 4);
    }
  inputFile.close();
  return 0;
}
