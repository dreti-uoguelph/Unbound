#pragma once
#include"stdafx.h"
#include"MemoryPool.h"

namespace Unbound
{
	class MemoryManager
	{
	public:
		static MemoryManager* get();

		MemoryPool textureDataPool; //The memory pool for storing texture data
		MemoryPool vertexDataPool; //The memory pool representing vertex data
		MemoryPool indexDataPool; //Memory pool for index data
		StagingMemoryPool vertexStagingPool; //The memory pool for staging vertices
		StagingMemoryPool indexStagingPool; //Memory pool for staging indices
		StagingMemoryPool textureStagingPool; //Memory pool for staging textures
	private:
		static MemoryManager* instance;
	};
}