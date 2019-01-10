#pragma once
#include"Vertex.h"

struct Model
{
	Vertex* vertices;
	uint32_t* indices;
	uint32_t numVertices, numIndices;
};