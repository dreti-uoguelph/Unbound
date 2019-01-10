#include"stdafx.h"
#include"MemoryManager.h"

namespace Unbound
{
	MemoryManager* MemoryManager::instance = nullptr;

	MemoryManager* MemoryManager::get()
	{
		if (instance == nullptr)
		{
			instance = new MemoryManager();
		}

		return instance;
	}
}