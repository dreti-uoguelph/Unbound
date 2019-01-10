#include "stdafx.h"
#include "MeshLoader.h"

namespace Unbound
{
	namespace IO
	{
		MeshLoader* MeshLoader::instance = nullptr;

		void MeshLoader::init()
		{
			instance = new MeshLoader();
		}

		MeshLoader* MeshLoader::get()
		{
			if (instance == nullptr)
			{
				instance = new MeshLoader();
			}

			return instance;
		}

		Graphics::Mesh MeshLoader::loadFromFile(std::string filePath, bool validationEnabled)
		{
			if (instance == nullptr)
			{
				instance = new MeshLoader();
			}

			std::ifstream inFile;
			inFile.open(filePath, std::ios::ate | std::ios::binary);
			uint64_t fileSize = 0;

			if (!inFile.is_open())
			{
				Logger::log("Error: could not load model: " + filePath);
				return Graphics::Mesh();
			}

			fileSize = inFile.tellg();
			inFile.seekg(0);

			//Check the magic number and file version if validation is enabled
			if (validationEnabled)
			{
				char magicNumber[4];
				float version;

				inFile.read(magicNumber, 4);

				if (strcmp(magicNumber, "umd") != 0)
				{
					Logger::log("Error: could not load corrupt model file: " + filePath);
					return Graphics::Mesh();
				}

				inFile.read(reinterpret_cast<char*>(&version), sizeof(float));

				if (version < 1.0)
				{
					Logger::log("Error: could not load outdated model file: " + filePath);
					return Graphics::Mesh();
				}
			}

			//Flush the contents of the existing mesh
			Graphics::Mesh toReturn;

			inFile.seekg(4 + sizeof(float));

			//Load the number of vertices and indices in the file
			inFile.read(reinterpret_cast<char*>(&toReturn.numVertices), sizeof(uint32_t));
			inFile.read(reinterpret_cast<char*>(&toReturn.numIndices), sizeof(uint32_t));

			//Read in all the vertices from the binary file
			toReturn.vertices = reinterpret_cast<Graphics::Vertex*>(MemoryManager::get()->vertexDataPool.addSection(toReturn.numVertices * sizeof(Graphics::Vertex)));
			inFile.read(reinterpret_cast<char*>(toReturn.vertices), toReturn.numVertices * sizeof(Graphics::Vertex));

			//Read all the indices from the binary file
			toReturn.indices = reinterpret_cast<uint32_t*>(MemoryManager::get()->indexDataPool.addSection(toReturn.numVertices * sizeof(uint32_t)));
			inFile.read(reinterpret_cast<char*>(toReturn.indices), toReturn.numIndices * sizeof(uint32_t));

			Logger::log("Loaded mesh: " + filePath);

			return toReturn;
		}

		MeshLoader::MeshLoader() noexcept
		{
		}

		MeshLoader::MeshLoader(std::string path)
		{
		}

		MeshLoader::~MeshLoader()
		{
		}
	}
}