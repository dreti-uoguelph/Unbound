#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"RenderBackend.h"
#include"glm\gtc\matrix_transform.hpp"
#include"Mesh.h"
#include"ImageLoader.h"
#include"MemoryManager.h"
#include"Model.h"
#include"Terrain.h"
#include"OpenGLStructs.h"
#include"OpenGLBuffer.h"
#include"OpenGLShader.h"
#include"OpenGLDebug.h"
#include"OpenGLPipeline.h"
#include"OpenGLTexture.h"

namespace Unbound
{
	namespace Graphics
	{
		class RenderBackendOpenGL : RenderBackend
		{
		public:
			//Called when the render backend is initialized
			virtual void init() override;
			//Called when the render backend draws a frame
			virtual void drawFrame() override;
			//Called when the render backend should be closed
			virtual void close() override;

			//Call to add new shaders to the renderer. Not recommended to call this during runtime as it is quite slow
			virtual void addShaders(std::vector<Graphics::Shader>& toAdd) override;
			//Add new vertex geometry to the renderer
			virtual void addGeometry(Mesh& toAdd) override;
			//Uploads new image data to the renderer
			virtual void addImageData(Image& toAdd) override;
			//Add terrain data to the render backend, using specialized buffers
			virtual void addTerrainData(Terrain& toAdd) override;

			//Update the structure sent to the vertex shader with new world, view and projection matrices
			virtual void updateMatrices(const glm::mat4& world, const glm::mat4& view, const glm::mat4& projection) override;
			//Update only the world matrix. This should usually be done once per object
			virtual void updateWorldMatrix(const glm::mat4& world) override;
			//Update only the view matrix. This only has to be done once per frame
			virtual void updateViewMatrix(const glm::mat4& view) override;
			//Update only the projection matrix. This only has to be done when camera properties change or when the window is resized
			virtual void updateProjectionMatrix(const glm::mat4& projection) override;

			//Get the next available image index, which is used for loading images into the renderer
			virtual const uint32_t& getNextFreeImageIndex() const override;

			/*Sets the multisample level used for the rendered framebuffer*/
			void setMultisampleLevel(short toSet);

			std::vector<Model>* models; //All the models that are currently in the scene

			RenderBackendOpenGL(const WindowInfo& windowInfoToSet, const std::vector<Shader>& shadersToSet);
			~RenderBackendOpenGL();
		private:

			WindowInfo& targetWindowInfo; //Information about the target window
			OpenGL::Buffers buffers; //The main opengl buffers used by the render backend
			OpenGL::Images images; //Variables related to opengl image handling and rendering
			OpenGL::Pipeline pipeline; //Data relating to the render pipeline used in this application
			OpenGL::Shaders shaders; //Data relating to the shaders used for rendering

			short multisampleLevel; //The level of multisampling to use for the intermediate framebuffer

			OpenGL::ShaderObserver<glm::mat4> storedMatrices[3]; //A copy of the world, view and projection matrices in the renderer
			std::vector<Shader>& shaderObjects; //All the shaders contained in the renderer. These contain abstract data, while opengl specific data is in its own struct

			/*Draws a single model to the screen*/
			void drawModel(Model& toDraw);
		};
	}
}
#endif