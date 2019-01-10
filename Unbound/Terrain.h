#pragma once
#include"stdafx.h"
#include"MemoryManager.h"
#include"Vertex.h"
#include"Material.h"
#include"ImageLoader.h"

namespace Unbound
{
	/*This class stored data about the terrain being rendered.
	Since the terrain is not expected to change frequently, it uses special pathways
	In the renderer to maximize performance for static objects*/
	class Terrain
	{
	public:

		/*This struct stores info about specific terrain chunks
		Each terrain consists of an array of chunks*/
		struct Chunk
		{
			Graphics::Vertex* vertices;
			uint32_t* indices;
		};

		/*Initialize the terrain, by generating vetices and loading data. If a cache file is found, it will be loaded by default*/
		void init(const bool loadFromCache = true);
		/*Rebuild the terrain after properties have changed*/
		void rebuild();

		/*Set the dimensions of the terrain, in terms of number of chunks*/
		void setNumChunks(const uint32_t widthToSet, const uint32_t heightToSet);
		/*Set the number of vertices in each chunk*/
		void setVerticesPerChunk(const uint32_t toSet);
		/*Set the terrain's material*/
		void setMaterial(const Graphics::TerrainMaterial& toSet);
		/*Set the terrain's heightmap. This is necessary for any non-flat terrain*/
		void setHeightmap(const Graphics::Image& toSet);
		/*Load a heightmap from a file and set it as the terrain's heightmap*/
		void setHeightmap(const std::string& fileToLoadFrom);
		/*Set the origin around which the terrain is centered*/
		void setOrigin(const glm::vec3& originToSet);
		/*Set the size of each terrain chunk in world units. Default is 6 metres squared*/
		void setChunkSize(const glm::vec2& chunkSizeToSet);

		/*Get the number of chunks in the terrain*/
		const uint32_t getNumChunks() const;
		/*Get the number of vertices per chunk*/
		const uint32_t getNumVerticesPerChunk() const;
		/*Get the terrain's material*/
		const Graphics::TerrainMaterial& getMaterial() const;
		/*Get the terrain's heightmap*/
		const Graphics::Image& getHeightMap() const;
		/*Get the total number of vertices in the terrain*/
		const uint32_t getNumVertices() const;
		/*Get the numeber of terrain indices*/
		const uint32_t getNumIndices() const;
		/*Get the number of indices in each chunk*/
		const uint32_t getNumIndicesPerChunk();
		/*Get the size of the terrain geometry (vertices + indices) in bytes*/
		const uint32_t getGeometrySizeBytes() const;
		/*Get the origin around which the terrain is centered*/
		const glm::vec3& getOrigin() const;
		/*Get the size of each terrain chunk*/
		const glm::vec2& getChunkSize() const;

		/*Get a pointer to the raw chunk data*/
		const Chunk* getData() const;

		Terrain() noexcept;
		~Terrain();
	private:

		uint32_t width, height; //The number of horizontal and vertical chunks in the terrain
		uint32_t verticesPerChunk; //The number of vertices per chunk, or density of the terrain
		uint32_t numChunks; //The number of chunks in the terrain
		uint32_t indicesPerChunk; //The number of indices per chunk
		uint32_t numTotalIndices; //The number of indices for drawing
		glm::vec2 chunkSize; //The size of each terrain chunk in world units
		Chunk* chunks; //The actual chunk data that will be generated

		glm::vec3 origin; //The point around which the terrain is centered

		Graphics::TerrainMaterial material; //The material that the terrain will be rendered with
		Graphics::Image heightmap; //The heightmap of the terrain, can be used to procedurally generate geometry

		/*Read the terrain cache into memory. Returns a bool indicating whether the file was read successfully*/
		const bool readCache();
		/*Write the terrain cache to the disk*/
		void writeCache();
	};
}