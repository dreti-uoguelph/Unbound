#pragma once
#include"stdafx.h"
#include<set>

namespace Unbound
{
	class AssetManager
	{
	public:
		static AssetManager* get();

		std::multiset<uint32_t> imageIndex;

		AssetManager() noexcept;
		~AssetManager();
	private:

		static AssetManager* instance;
	};
}