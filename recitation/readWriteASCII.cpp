#include <iostream>
#include <fstream>

using namespace std;

int main() {

	ofstream outputFile( "recitationASCII.txt" );
	float data = 255;
	outputFile << data;
	outputFile.close();
	
	ifstream inputFile( "recitationASCII.txt" );
	float dataIn = 0;
	inputFile >> dataIn;
	cout << "Value read in from recitationASCII.txt file: " << dataIn << endl;
	inputFile.close();
	
	return 0;
}
