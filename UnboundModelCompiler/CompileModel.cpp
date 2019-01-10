#include "stdafx.h"
#include "CompileModel.h"

void compileModel(string path, int tnum, int* numErrors)
{
	//The output file will have the same name, just with a .m extension
	string writePath = path.substr(0, path.find_last_of('.'));
	writePath += ".m";
	writeModel(writePath, loadModel(path, tnum, numErrors), tnum, numErrors);
}