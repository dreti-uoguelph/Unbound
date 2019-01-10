#pragma once
#include"stdafx.h"
#include<fstream>
#include"Model.h"

Model loadModel(string filePath, int tnum, int* numErrors);