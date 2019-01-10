#pragma once
#include"stdafx.h"
#include"Model.h"
#include<fstream>

void writeModel(string path, Model toWrite, int tnum, int* numErrors);