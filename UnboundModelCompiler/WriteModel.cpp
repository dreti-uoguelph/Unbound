#include "stdafx.h"
#include "WriteModel.h"

void writeModel(string path, Model toWrite, int tnum, int* numErrors)
{
	ofstream outFile;
	float version = 1.0;

	outFile.open(path, ios::binary);

	if (!outFile.is_open())
	{
		cout << tnum << ">	" << " could not open ouput file: " << path << endl;
		cout << tnum << ">	" << " skipping..." << endl;
		*numErrors++;
	}

	//Write header of binary file. Includes a unique program name and the version number of the file format
	outFile.write("umd", 4);
	outFile.write(reinterpret_cast<const char*>(&version), sizeof(float));

	//Write the number of vertices and the number of indices
	outFile.write(reinterpret_cast<const char*>(&toWrite.numVertices), sizeof(uint32_t));
	outFile.write(reinterpret_cast<const char*>(&toWrite.numIndices), sizeof(uint32_t));

	//Write the vertex and index arrays
	outFile.write(reinterpret_cast<const char*>(toWrite.vertices), toWrite.numVertices * sizeof(Vertex));
	outFile.write(reinterpret_cast<const char*>(toWrite.indices), toWrite.numIndices * sizeof(uint32_t));

	cout << tnum << ">" << " Done writing: " << path << endl;
	outFile.close();
}