#pragma once
#include"stdafx.h"

namespace Unbound
{
	namespace IO
	{
		class FileSystem
		{
		public:

			FileSystem();
			FileSystem(const std::string& homeDirToSet);

		private:
			static FileSystem* instance;
			std::string homeDir;
			std::string audioDir, modelsDir, scenesDir, shadersDir, texturesDir;

			static const FileSystem* get();
		};
	}
}