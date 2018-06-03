#include <iostream>
#include <fstream>
#include <chrono>
#include <sys/stat.h>
#include<vector>
#include <algorithm>
#include <locale> 

using namespace std;

// used for holding file stats
struct stat size;

// hold start time value
chrono::high_resolution_clock::time_point start;

// hold stop time value
chrono::high_resolution_clock::time_point stop;

// container for all records
// Each record is held in a string vector
vector<vector<string>> records;

// Parser function declaration
void parse();

// Serialization function declaration
void serialize();

// DeSerialization function declaration
void deSerialize();

// Process timer function declaration
void timer(bool click);

// File size function declaration
void printSize(string fileName);

// Rec Data function declaration
vector<string> storeRecData(string line);

// Byte array builder function declaration
string getRecBytes(vector<string> record);

// Main method
int main(){
	parse();
	serialize();
	deSerialize();
	return 0;
}

// Start and stop the timer and print duration
void timer(bool click, string process)
{
	// Record serialization start time
	if(click)
	{
		start = chrono::high_resolution_clock::now();
	}
	// Record serialization end time
	else
	{
		stop = chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> duration = stop - start;
		cout << "Elapsed time(" << process << "): " << duration.count() << " milliseconds \n";
	}
}

// Print the given file size
void printSize(string fileName)
{
	stat(fileName.c_str(), &size);
	cout << "File size(" << fileName << "): " << size.st_size << " bytes \n";
}


// Extracts data from given line and stores in a string vector
// Each string vector represents a record
vector<string> storeRecData(string line)
{
	vector<string> recData;

	// used to determine record type
	int commaCnt = count(line.begin(), line.end(), ',');

	// allocates memory for rec type
	string recType = "recordTypePlaceHolder";
	recData.push_back(recType);

    //tokenize input string and push into vector of records
	char del = ',';
	int pos = 0;
	string token;
	while ((pos = line.find(del)) != std::string::npos) {
		token = line.substr(0, pos);
		recData.push_back(token);
		line.erase(0, pos + 1);
	}
	recData.push_back(line);

    // determine record type
	locale loc;
	char check = recData[recData.size()-1][0];
	if (commaCnt == 5)
	{
		recData[0] = "quote ";
	}
	else
	{
		if (isdigit(check,loc))
		{
			recData[0] = "signal";
		}
		else
		{
			recData[0] = "trade ";
		}
	}

	return recData;
}

// returns byte array from input record for writing to binary file
string getRecBytes(vector<string> record)
{
	string recBytes = "";
	for(int i = 0; i < record.size(); i++) 
	{
		if (i == record.size() - 1)
		{
			recBytes = recBytes + record[i];
		}
		else
		{
			recBytes = recBytes + record[i] + ',';
		}
	}
	return recBytes;
}

// Function that parses the csv
void parse()
{
	printSize("q.csv");

    //open input stream from given file
	ifstream qIn("q.csv");

	//check if file can be opened
	if(!qIn.is_open()) cout << "ERROR: Cannot open file!" << '\n';

	//String to hold each line of 
	string line;

    // start timer
	timer(true,"");

	//iterate through every line in .csv, convert each character into byte and write to binary file
	while(qIn.good()){
		getline(qIn,line);
		if(!line.empty())
		{
			records.push_back(storeRecData(line));
		}
	}

    // stop timer and print
	timer(false,"Parsing");

	//close both files
	qIn.close();
}

// Serialization function
void serialize()
{
	//open output stream to binary file to hold binary data
	ofstream UBM3("UBM3.bin", ios::out | ios::binary);

	//open output stream to binary file to hold binary data
	ofstream ZBM3("ZBM3.bin", ios::out | ios::binary);

	//open output stream to binary file to hold binary data
	ofstream ZFM3("ZFM3.bin", ios::out | ios::binary);

	//open output stream to binary file to hold binary data
	ofstream ZNM3("ZNM3.bin", ios::out | ios::binary);

	//open output stream to binary file to hold binary data
	ofstream ZTM3("ZTM3.bin", ios::out | ios::binary);

    // start timer
	timer(true,"");

    // write to binary file by symbol
	for(int i = 0; i < records.size(); i++) {
		string symbol = records[i][2];
		string recBytes = getRecBytes(records[i]);
		int size = recBytes.size();
		const char* line = recBytes.c_str();

		if(symbol.compare("UBM3") == 0)
		{
			UBM3.write(line, size);
		}
		else if(symbol.compare("ZBM3") == 0)
		{
			ZBM3.write(line, size);
		}
		else if(symbol.compare("ZFM3") == 0)
		{
			ZFM3.write(line, size);
		}
		else if(symbol.compare("ZNM3") == 0)
		{
			ZNM3.write(line, size);
		}
		else if(symbol.compare("ZTM3") == 0)
		{
			ZTM3.write(line, size);
		}
	}

    //stop timer and print
	timer(false,"Serialization");

    // Close output streams
	UBM3.close();
	ZBM3.close();
	ZFM3.close();
	ZNM3.close();
	ZTM3.close();

    // print sizes of bin files
	printSize("UBM3.bin");
	printSize("ZBM3.bin");
	printSize("ZFM3.bin");
	printSize("ZNM3.bin");
	printSize("ZTM3.bin");
}

// Deserialization function
void deSerialize()
{
	//open input stream from newly created binary file
	ifstream UBM3("UBM3.bin", ios::binary);

	//open input stream from newly created binary file
	ifstream ZBM3("ZBM3.bin", ios::binary);

	//open input stream from newly created binary file
	ifstream ZFM3("ZFM3.bin", ios::binary);

	//open input stream from newly created binary file
	ifstream ZNM3("ZNM3.bin", ios::binary);

	//open input stream from newly created binary file
	ifstream ZTM3("ZTM3.bin", ios::binary);

	//open output stream to output csv, which should be identical to original
	ofstream csvOut( "output.csv", ios::binary );

	//open output stream to textfile
	//will contain record types in order
	ofstream recTypTxt( "recordTypes.txt", ios::binary );

    // start timer
	timer(true,"");

    // allocate memory for bytes to write
	char *memblock, *recTypByts;

    //read binary file by symbol
	for(int i = 0; i < records.size(); i++) {
		string symbol = records[i][2];
		string recBytes = getRecBytes(records[i]);
		int size = recBytes.size() - 7;
		int recTypSiz = 7;
		memblock = new char [size];
		recTypByts = new char [recTypSiz];

		if(symbol.compare("UBM3") == 0)
		{
			UBM3.read(recTypByts,recTypSiz);
			UBM3.read(memblock,size);
		}
		else if(symbol.compare("ZBM3") == 0)
		{
			ZBM3.read(recTypByts,recTypSiz);
			ZBM3.read(memblock,size);
		}
		else if(symbol.compare("ZFM3") == 0)
		{
			ZFM3.read(recTypByts,recTypSiz);
			ZFM3.read(memblock,size);
		}
		else if(symbol.compare("ZNM3") == 0)
		{
			ZNM3.read(recTypByts,recTypSiz);
			ZNM3.read(memblock,size);
		}
		else if(symbol.compare("ZTM3") == 0)
		{
			ZTM3.read(recTypByts,recTypSiz);
			ZTM3.read(memblock,size);
		}

		// assemble output csv
		csvOut.write(memblock, size);

		// assemble rec type txt
		string num = to_string(i+1)+")", newLin = "     \n";
		int numSiz = num.size(), newLinSiz = newLin.size();
		recTypTxt.write(num.c_str(),numSiz);
		recTypTxt.write(recTypByts,recTypSiz - 1);
		recTypTxt.write(newLin.c_str(),newLinSiz);
	}

    //stop timer and print
	timer(false,"Deserialization");

    //close all files
	UBM3.close();
	ZBM3.close();
	ZFM3.close();
	ZNM3.close();
	ZTM3.close();	
	csvOut.close();
	recTypTxt.close();

    //print file size
	printSize("output.csv");
}