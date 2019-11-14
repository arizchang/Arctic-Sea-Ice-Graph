#include <iostream>
#include <fstream>

using namespace std;

int main() {

	/**********
	* Predefined macros listed in GNU C's "stdint.h". NOTE: Some systems don't have this file. 
	* https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
	**********/
	/*
	if( __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ )
		cout << "System uses little endian\n";
	else
		cout << "System uses big endian\n";
		
	if( __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__ )
		cout << "System's floating point number uses little endian\n";
	else
		cout << "System's floating point number uses big endian\n";
	
	cout << "Number of bytes in system's floating point number: " << __SIZEOF_FLOAT__ << endl;
	cout << "Can also use sizeof(float) to figure this out: " << sizeof(float) << endl;
	*/
	
	ofstream outputFile( "recitationBinary", ios::out | ios::binary );
	float data = 255;
	outputFile.write( (char*)&data , 4 ); // Write 4 bytes of data
	outputFile.close();
	
	ifstream inputFile( "recitationBinary", ios::in | ios::binary );
	float dataIn = 0;
	inputFile.read( (char*)&dataIn, 4 ); // Read 4 bytes of data
	cout << "Value read in from recitationBinary file: " << dataIn << endl;
	inputFile.close();
	
	return 0;
}
