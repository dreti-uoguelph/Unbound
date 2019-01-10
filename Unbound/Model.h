#pragma once
#include"stdafx.h"
#include"MeshLoader.h"
#include"Material.h"

namespace Unbound
{
	/*A model which can be added to a scene. Includes all characteristics needed to draw it properly*/
	class Model : public GameObject
	{
	public:
		/*Initialize this gameobject. Ignored for static models*/
		virtual void init() override;
		/*Update the model. Ignored for static models*/
		virtual void update() override;
		/*Clean up the model and associated data*/
		virtual void close() override;

		/*Load the complete model, including metadata from a file*/
		void loadFromFile(std::string path);

		Graphics::Mesh mesh;
		Graphics::Material material;
		Transform transform;

		Model() noexcept;
		~Model();

	private:
	};
}