#pragma once
#include"stdafx.h"
#include"Shader.h"
#include"Window.h"
#include"Vertex.h"
#include"Mesh.h"
#include"MemoryPool.h"
#include"Image.h"
#include"Terrain.h"

namespace Unbound
{
	namespace Graphics
	{
		enum RenderBackendType { RENDER_BACKEND_VULKAN, RENDER_BACKEND_OPENGL };

		class RenderBackend
		{
		public:
			//Called when the render backend is initialized
			virtual void init() = 0;
			//Called when the render backend should draw a frame
			virtual void drawFrame() = 0;
			//Called when the render backend should be closed
			virtual void close() = 0;
			//Get the type of the render backend
			RenderBackendType getType();

			//Add shaders to the render backend
			virtual void addShaders(std::vector<Graphics::Shader>& toAdd) = 0;
			//Uploads new geometry to the render backend
			virtual void addGeometry(Mesh& toAdd) = 0;
			//Adds image data to the render backend
			virtual void addImageData(Image& toAdd) = 0;
			//Adds terrain data to the render backend. This is stored in a separate buffer which is never unloaded
			virtual void addTerrainData(Terrain& toAdd) = 0;

			//Update the structure sent to the vertex shader with new world, view and projection matrices
			virtual void updateMatrices(const glm::mat4& world, const glm::mat4& view, const glm::mat4& projection);
			//Update only the world matrix. This should usually be done once per object
			virtual void updateWorldMatrix(const glm::mat4& world);
			//Update only the view matrix. This only has to be done once per frame
			virtual void updateViewMatrix(const glm::mat4& view);
			//Update only the projection matrix. This only has to be done when camera properties change or when the window is resized
			virtual void updateProjectionMatrix(const glm::mat4& projection);

			//Get the next available image index, which is used for loading images into the renderer
			virtual const uint32_t& getNextFreeImageIndex() const;

			bool initialized; //Whether the render backend has been fully initialized

			RenderBackend() noexcept;
			~RenderBackend();
		private:
			RenderBackendType backendType; //The type of render backend this is
			uint32_t temp; //Temporary variable
		};
	}
}