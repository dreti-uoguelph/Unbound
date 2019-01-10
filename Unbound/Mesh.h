#pragma once
#include"stdafx.h"
#include"GameObject.h"
#include"Vertex.h"
#include"Transform.h"
#include"Material.h"

namespace Unbound
{
	namespace Graphics
	{
		class Mesh
		{
		public:
			Vertex * vertices;
			uint32_t* indices;
			uint32_t numVertices, numIndices;
			uint32_t bufferStartIndex;
			uint32_t vertexOffset;

			/*The update frequency of this geometry. Allows it to be placed in the most appropriate memory*/
			enum MeshType { MESHTYPE_STATIC, MESHTYPE_DYNAMIC, MESHTYPE_STREAM };
			MeshType meshType;

			Mesh() noexcept;
			~Mesh();
		private:
		};
	}
}