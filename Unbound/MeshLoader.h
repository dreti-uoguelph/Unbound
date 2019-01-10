#pragma once
#include"stdafx.h"
#include<fstream>
#include"Mesh.h"
#include"MemoryPool.h"
#include"Logger.h"

namespace Unbound
{
	namespace IO
	{
		/*Loads binary meshes from a compiled binary file to a memory pool*/
		class MeshLoader
		{
		public:
			void init();

			//Loads a mesh from a loose binary file
			static Graphics::Mesh loadFromFile(std::string filePath, bool validationEnabled = true);
			//Get the current MeshLoader instance
			static MeshLoader* get();

			MeshLoader() noexcept;
			MeshLoader(std::string path);
			~MeshLoader();
		private:
			static MeshLoader* instance;
		};
	}
}