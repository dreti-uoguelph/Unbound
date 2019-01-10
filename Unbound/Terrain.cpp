#include"stdafx.h"
#include"Terrain.h"

namespace Unbound
{
	/*Initialize the terrain, by generating vetices and loading data*/
	void Terrain::init(const bool loadFromCache)
	{
		if (loadFromCache)
		{
			if (readCache())
			{
				return;
			}
		}
		else
		{
			Logger::log("Skipping terrain cache...");
		}

		Logger::log("Generating new terrain...");

		//Check dimensions and allocate memory for vertices
		if (width == 0 || height == 0 || numChunks == 0 || verticesPerChunk == 0)
		{
			Logger::log("Error: cannot create a terrain with dimensions equaling zero");
			return;
		}

		chunks = new Chunk[width * height];

		const uint32_t vertsPerSide = static_cast<uint32_t>(sqrt(verticesPerChunk));
		const uint32_t numIndices = ((vertsPerSide - 1) * (vertsPerSide - 1)) * 6;
		numTotalIndices = numIndices * numChunks;

		float offsetX = 0;
		float offsetZ = 0;
		uint32_t indexOffset = 0;

		//Generate a grid of flat vertices
		for (uint32_t i = 0; i < numChunks; i++)
		{
			chunks[i].vertices = new Graphics::Vertex[verticesPerChunk * 3];
			chunks[i].indices = new uint32_t[numIndices * (3 + (verticesPerChunk / 16))];
			memset(chunks[i].vertices, 0, verticesPerChunk * sizeof(Graphics::Vertex));
			memset(chunks[i].indices, 0, numIndices * sizeof(uint32_t));

			uint32_t l = 0;
			for (uint32_t j = 0; j < vertsPerSide; j++)
			{
				for (uint32_t k = 0; k < vertsPerSide; k++)
				{
					chunks[i].vertices[l].color = glm::vec3(1.0f, 1.0f, 1.0f);
					chunks[i].vertices[l].normal = glm::vec3(0.0f, 1.0f, 0.0f);
					chunks[i].vertices[l].position = glm::vec3(offsetX + ((k / (float)vertsPerSide) * chunkSize.x), -origin.y, offsetZ + ((j / (float)vertsPerSide) * chunkSize.y));
					chunks[i].vertices[l].uv = glm::vec2((k / (float)(vertsPerSide - 1)) * chunkSize.x, (j / (float)(vertsPerSide - 1)) * chunkSize.y);
					chunks[i].vertices[l].uv2 = glm::vec2(offsetX + (k / (numChunks * (float)(vertsPerSide - 1))), offsetZ + (j / (numChunks * (float)(vertsPerSide - 1))));
					l++;
				}
			}

			//Generate indices row by row
			for (uint32_t m = 0; m < vertsPerSide - 1; m++)
			{
				uint32_t k = 0;
				uint32_t vOffset = vertsPerSide * m;
				uint32_t iOffset = ((vertsPerSide - 1) * 6) * m;

				for (uint32_t j = 0; k < iOffset + vertsPerSide - 1; j++)
				{
					chunks[i].indices[iOffset + j] = vOffset + k;
					chunks[i].indices[iOffset + j + 1] = vOffset + k + 1;
					chunks[i].indices[iOffset + j + 2] = vOffset + vertsPerSide + k;
					j += 2;
					k++;
				}

				k = 0;
				for (uint32_t j = (3 * (vertsPerSide - 1)); k < vertsPerSide - 1; j++)
				{
					chunks[i].indices[iOffset + j] = vOffset + k + 1;
					chunks[i].indices[iOffset + j + 1] = vOffset + vertsPerSide + k + 1;
					chunks[i].indices[iOffset + j + 2] = vOffset + vertsPerSide + k;
					j += 2;
					k++;
				}
			}

			if (i % width == 0 && i != 0)
			{
				offsetZ += chunkSize.y;
				offsetX = 0;
			}
			else
			{
				offsetX += chunkSize.x;
			}

			indexOffset += numIndices;
		}

		//Write the terrain cache
		if (loadFromCache)
		{
			Logger::log("Writing terrain cache...");
			writeCache();
		}
	}

	/*Rebuild the terrain after properties have changed*/
	void Terrain::rebuild()
	{
	}

	/*Set the dimensions of the terrain, in terms of number of chunks*/
	void Terrain::setNumChunks(const uint32_t widthToSet, const uint32_t heightToSet)
	{
		width = widthToSet;
		height = heightToSet;
		numChunks = width * height;
	}

	/*Set the number of vertices in each chunk*/
	void Terrain::setVerticesPerChunk(const uint32_t toSet)
	{
		verticesPerChunk = toSet;
	}

	/*Set the terrain's material*/
	void Terrain::setMaterial(const Graphics::TerrainMaterial& toSet)
	{
		material = toSet;
	}

	/*Set the terrain's heightmap. This is necessary for any non-flat terrain*/
	void Terrain::setHeightmap(const Graphics::Image& toSet)
	{
		heightmap = toSet;
	}

	/*Load a heightmap from a file and set it as the terrain's heightmap*/
	void Terrain::setHeightmap(const std::string& fileToLoadFrom)
	{
		heightmap = IO::ImageLoader::loadFromFile(fileToLoadFrom);
	}

	/*Set the size of each terrain chunk in world units. Default is 4 metres squared*/
	void Terrain::setChunkSize(const glm::vec2& chunkSizeToSet)
	{
		chunkSize = chunkSizeToSet;
	}

	/*Get the number of chunks in the terrain*/
	const uint32_t Terrain::getNumChunks() const
	{
		return numChunks;
	}

	/*Get the number of vertices per chunk*/
	const uint32_t Terrain::getNumVerticesPerChunk() const
	{
		return verticesPerChunk;
	}

	/*Get the terrain's material*/
	const Graphics::TerrainMaterial& Terrain::getMaterial() const
	{
		return material;
	}

	/*Get the terrain's heightmap*/
	const Graphics::Image& Terrain::getHeightMap() const
	{
		return heightmap;
	}

	/*Get the total number of vertices in the terrain*/
	const uint32_t Terrain::getNumVertices() const
	{
		return numChunks * verticesPerChunk;
	}

	/*Get the numeber of terrain indices*/
	const uint32_t Terrain::getNumIndices() const
	{
		return numTotalIndices;
	}

	/*Get the size of the terrain vertices in bytes*/
	const uint32_t Terrain::getGeometrySizeBytes() const
	{
		return (getNumVertices() * sizeof(Graphics::Vertex)) + (getNumIndices() * sizeof(uint32_t));
	}

	/*Set the origin around which the terrain is centered*/
	void Terrain::setOrigin(const glm::vec3& originToSet)
	{
		origin = originToSet;
	}

	//Read the terrain cache into memory
	const bool Terrain::readCache()
	{
		//Look for a cache file and copy the vertices from the file into memory if it is found
		std::fstream terrainCacheFile;
		terrainCacheFile.open("terraincache", std::ios::binary | std::ios::in | std::ios::ate);

		Logger::log("Reading terrain cache...");

		if (terrainCacheFile.is_open())
		{
			uint64_t fileSize = static_cast<uint64_t>(terrainCacheFile.tellg());
			terrainCacheFile.seekg(0);

			//Read in the number of chunks in the terrain (x and y)
			terrainCacheFile.read(reinterpret_cast<char*>(&width), sizeof(uint32_t));
			terrainCacheFile.read(reinterpret_cast<char*>(&height), sizeof(uint32_t));
			numChunks = width * height;

			//Read in the number of vertices per chunk
			terrainCacheFile.read(reinterpret_cast<char*>(&verticesPerChunk), sizeof(uint32_t));

			//Read the number of indices per chunk
			terrainCacheFile.read(reinterpret_cast<char*>(&indicesPerChunk), sizeof(uint32_t));

			//Read in the origin of the terrain
			terrainCacheFile.read(reinterpret_cast<char*>(&origin), sizeof(glm::vec3));

			//Read the world space size of each chunk
			terrainCacheFile.read(reinterpret_cast<char*>(&chunkSize), sizeof(glm::vec2));

			//Allocate memory for the chunks
			chunks = new Chunk[width * height];

			//Allocate memory for the vertices and indices in each chunk
			for (uint32_t i = 0; i < numChunks; i++)
			{
				chunks[i].vertices = new Graphics::Vertex[verticesPerChunk];
				chunks[i].indices = new uint32_t[indicesPerChunk];
			}

			//Copy the vertices and indices from the cache into the chunks
			for (uint32_t i = 0; i < numChunks; i++)
			{
				terrainCacheFile.read(reinterpret_cast<char*>(chunks[i].vertices), sizeof(Graphics::Vertex) * verticesPerChunk);
				terrainCacheFile.read(reinterpret_cast<char*>(chunks[i].indices), sizeof(uint32_t) * indicesPerChunk);
			}

			//Close the file and return
			terrainCacheFile.close();
			return true;
		}

		Logger::log("Warning: could not read terrain cache, regenerating...");

		terrainCacheFile.close();

		return false;
	}

	//Write the terrain cache to the disk
	void Terrain::writeCache()
	{
		std::fstream terrainCacheFile;
		terrainCacheFile.open("terraincache", std::ios::binary | std::ios::out);

		if (!terrainCacheFile.is_open())
		{
			Logger::log("Error: could not open terrain cache for writing");
			return;
		}

		//Write the number of chunks in the terrain (x and y)
		terrainCacheFile.write(reinterpret_cast<char*>(&width), sizeof(uint32_t));
		terrainCacheFile.write(reinterpret_cast<char*>(&height), sizeof(uint32_t));

		//Write the number of vertices per chunk
		terrainCacheFile.write(reinterpret_cast<char*>(&verticesPerChunk), sizeof(uint32_t));

		//Write the number of indices per chunk
		terrainCacheFile.write(reinterpret_cast<char*>(&indicesPerChunk), sizeof(uint32_t));

		//Write in the origin of the terrain
		terrainCacheFile.write(reinterpret_cast<char*>(&origin), sizeof(glm::vec3));

		//Write the world space size of each chunk
		terrainCacheFile.write(reinterpret_cast<char*>(&chunkSize), sizeof(glm::vec2));

		//Write the contents of all the chunks
		for (uint32_t i = 0; i < numChunks; i++)
		{
			terrainCacheFile.write(reinterpret_cast<char*>(chunks[i].vertices), sizeof(Graphics::Vertex) * verticesPerChunk);
			terrainCacheFile.write(reinterpret_cast<char*>(chunks[i].indices), sizeof(uint32_t));
		}

		terrainCacheFile.close();
	}

	/*Get the number of indices in each chunk*/
	const uint32_t Terrain::getNumIndicesPerChunk()
	{
		indicesPerChunk = getNumIndices() / getNumChunks();
		return indicesPerChunk;
	}

	/*Get a pointer to the raw chunk data*/
	const Terrain::Chunk* Terrain::getData() const
	{
		return chunks;
	}

	/*Get the origin around which the terrain is centered*/
	const glm::vec3& Terrain::getOrigin() const
	{
		return origin;
	}

	/*Get the size of each terrain chunk*/
	const glm::vec2& Terrain::getChunkSize() const
	{
		return chunkSize;
	}

	Terrain::Terrain() noexcept
	{
		width = 1;
		height = 1;
		numChunks = 1;
		verticesPerChunk = 4;
		indicesPerChunk = 6;
		numTotalIndices = 6;
		origin = glm::vec3(0.0f, -1.0f, 0.0f);
		chunkSize = glm::vec2(4.0f, 4.0f);
	}

	Terrain::~Terrain()
	{
		delete[] chunks;
	}
}