#include "stdafx.h"

namespace Unbound
{
	namespace IO
	{
		FileSystem* FileSystem::instance = nullptr;

		const FileSystem* FileSystem::get()
		{
			if (instance == nullptr)
			{
				instance = new FileSystem();
			}

			return instance;
		}

		FileSystem::FileSystem()
		{
		}

		FileSystem::FileSystem(const std::string& homeDirToSet) : homeDir(homeDirToSet)
		{
		}
	}
}