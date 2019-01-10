#include"stdafx.h"
#include"AssetManager.h"

namespace Unbound
{
	AssetManager* AssetManager::instance = nullptr;

	AssetManager* AssetManager::get()
	{
		if (instance == nullptr)
		{
			instance = new AssetManager();
		}

		return instance;
	}

	AssetManager::AssetManager() noexcept
	{
	}

	AssetManager::~AssetManager()
	{
	}
}