#pragma once
#include"stdafx.h"

/*Models are made of vertices. Each one is 64 bytes in size for alignment*/
struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 color;
	glm::vec2 uv;
	glm::vec2 uv2;
};