#pragma once
#include"stdafx.h"
#include<fstream>
#include"MeshLoader.h"

namespace Unbound
{
	class Scene
	{
	public:
		void load(std::string name);
		void unload();
	private:
	};
}