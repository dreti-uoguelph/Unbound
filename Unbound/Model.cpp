#include"stdafx.h"
#include"Model.h"

namespace Unbound
{
	/*Initialize this gameobject. Ignored for static models*/
	void Model::init()
	{
	}

	/*Update the model. Ignored for static models*/
	void Model::update()
	{
	}

	/*Clean up the model and associated data*/
	void Model::close()
	{
	}

	/*Load the complete model, including metadata from a file*/
	void Model::loadFromFile(std::string path)
	{
	}

	Model::Model() noexcept
	{
	}

	Model::~Model()
	{
	}
}