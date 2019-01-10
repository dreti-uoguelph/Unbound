#include "stdafx.h"
#include "Scene.h"

namespace Unbound
{
	void Scene::load(std::string name)
	{
		std::ifstream inFile;
		inFile.open("//Assets//Scenes//" + name);

		if (!inFile.is_open())
		{
			throw std::runtime_error("Could not open scene");
		}

		inFile.close();
	}

	void Scene::unload()
	{
	}
}