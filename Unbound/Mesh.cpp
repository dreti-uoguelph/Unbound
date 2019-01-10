#include "stdafx.h"
#include "Mesh.h"

namespace Unbound
{
	namespace Graphics
	{
		Mesh::Mesh() noexcept
		{
			vertices = nullptr;
			indices = nullptr;
			numVertices = 0;
			numIndices = 0;
			bufferStartIndex = 0;
			vertexOffset = 0;
			meshType = MeshType::MESHTYPE_STATIC;
		}

		Mesh::~Mesh()
		{
		}
	}
}