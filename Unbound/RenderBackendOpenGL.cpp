#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"RenderBackendOpenGL.h"

namespace Unbound
{
	namespace Graphics
	{
		//Called when the render backend is initialized
		void RenderBackendOpenGL::init()
		{
#ifdef _DEBUG
			OpenGL::setupErrorCallback();
#endif
			//Setup common shader bindings
			OpenGL::Defaults::worldViewProjection.init();

			//Create and bind the VAO
			buffers.vertexArrayObject = OpenGL::createNewVertexArrayObject();

			//Create a new vertex buffer object and store it in the right struct
			buffers.vertexBufferObject = OpenGL::createNewVertexBuffer();

			//Create and bind the index buffer object
			buffers.indexBufferObject = OpenGL::createNewIndexBuffer();

			//Compile all the shaders that are currently part of the renderer
			shaders.storedShaderObjects.push_back(OpenGL::createNewShader(shaderObjects[0]));
			shaders.storedShaderObjects.push_back(OpenGL::createNewShader(shaderObjects[1]));

			//Link the first two shaders into a program and activate it
			shaders.openglShaderPrograms.push_back(OpenGL::createNewShaderProgram(shaders, shaders.storedShaderObjects[0], shaders.storedShaderObjects[1]));
			OpenGL::activateShaderProgram(shaders.openglShaderPrograms[0]);

			//Set up the vertex layout for opengl
			OpenGL::setupOpenGLVertexLayout();

			//Setup culling
			OpenGL::setupCulling();

			//Setup depth test
			OpenGL::enableDepthTest();

			//Setup intermediate buffers
			Mesh renderRect = IO::MeshLoader::loadFromFile("./Assets/Models/rect.m");

			//Buffer the vertex data from the mesh into the currently bound vertex buffer
			buffers.postprocessvbo = OpenGL::createNewVertexBuffer();
			buffers.postprocessvao = OpenGL::createNewVertexArrayObject();
			buffers.postprocessibo = OpenGL::createNewIndexBuffer();
			OpenGL::bindBuffers(buffers.postprocessvbo, buffers.postprocessvao, buffers.postprocessibo);
			OpenGL::setupOpenGLVertexLayout();
			glBufferData(GL_ARRAY_BUFFER, renderRect.numVertices * sizeof(Vertex), renderRect.vertices, GL_STATIC_DRAW);

			//Buffer the indices used for rendering
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderRect.numIndices * sizeof(uint32_t), renderRect.indices, GL_STATIC_DRAW);

			//Load the post process shader program
			shaderObjects.push_back(Shader("./Assets/Shaders/PostProcessVertexPassthroughGL.vert", SHADER_TYPE_VERTEX));
			shaders.storedShaderObjects.push_back(OpenGL::createNewShader(shaderObjects.back()));
			shaderObjects.push_back(Shader("./Assets/Shaders/PostProcessStackGL.frag", SHADER_TYPE_FRAGMENT));
			shaders.storedShaderObjects.push_back(OpenGL::createNewShader(shaderObjects.back()));
			shaders.openglShaderPrograms.push_back(OpenGL::createNewShaderProgram(shaders, shaders.storedShaderObjects[2], shaders.storedShaderObjects.back()));

			//Setup post process pipeline
			OpenGL::setupPostProcessingFramebuffer(buffers, images, targetWindowInfo.displayWidth, targetWindowInfo.displayHeight, multisampleLevel);

			//Load the blank image
			Image blankImage = IO::ImageLoader::loadFromFile("./Assets/Textures/blank.png");
			images.blankImage = OpenGL::createNewTexture(blankImage.getWidth(), blankImage.getHeight(), blankImage.getData());
		}

		//Called when the render backend draws a frame
		void RenderBackendOpenGL::drawFrame()
		{
			//Clear the color and depth buffers
			OpenGL::clearScreen();

			//Bind the render output framebuffer for raw rendering
			//glBindFramebuffer(GL_FRAMEBUFFER, buffers.postProcessFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, buffers.postProcessFrameBuffer);
			OpenGL::clearScreen();

			//Start by binding the main program and default material
			OpenGL::activateShaderProgram(shaders.openglShaderPrograms[0]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, images.blankImage);

			//Bind the terrain buffers and draw the terrain first
			if (pipeline.terrainExists)
			{
				OpenGL::bindBuffers(buffers.terrainvbo, buffers.terrainvao, buffers.terrainabo);
				updateWorldMatrix(glm::mat4());
				glDrawElements(GL_TRIANGLES, pipeline.numTerrainIndices, GL_UNSIGNED_INT, 0);
			}

			//Bind the main buffers and render each model
			OpenGL::bindBuffers(buffers.vertexBufferObject, buffers.vertexArrayObject, buffers.indexBufferObject);

			//Draw every model in the scene
			for (auto i : *models)
			{
				drawModel(i);
			}

			//Bind the final render framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//Bind the post process program and associated values
			OpenGL::activateShaderProgram(shaders.openglShaderPrograms[1]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, images.renderedOutput);

			//Draw a full screen quad with the final render output
			OpenGL::bindBuffers(buffers.postprocessvbo, buffers.postprocessvao, buffers.postprocessibo);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			//Finish and swap buffers
			glfwSwapBuffers(targetWindowInfo.window);
		}

		//Called when the render backend should be closed
		void RenderBackendOpenGL::close()
		{
			//Release all shaders
			OpenGL::releaseAllShaders(shaders.storedShaderObjects);
		}

		//Call to add new shaders to the renderer. Not recommended to call this during runtime as it is quite slow
		void RenderBackendOpenGL::addShaders(std::vector<Graphics::Shader>& toAdd)
		{
			//Create and compile all the new shaders being added
			for (auto i : toAdd)
			{
				shaders.storedShaderObjects.push_back(OpenGL::createNewShader(i));
			}
		}

		//Add new vertex geometry to the renderer
		void RenderBackendOpenGL::addGeometry(Mesh& toAdd)
		{
			GLenum drawtype;

			switch (toAdd.meshType)
			{
			case Mesh::MESHTYPE_DYNAMIC:
				drawtype = GL_DYNAMIC_DRAW;
				break;
			case Mesh::MESHTYPE_STREAM:
				drawtype = GL_STREAM_DRAW;
				break;
			default:
				drawtype = GL_STATIC_DRAW;
				break;
			}

			//Bind the buffers suitable for the model type being added
			OpenGL::bindBuffers(buffers.vertexBufferObject, buffers.vertexArrayObject, buffers.indexBufferObject);

			//Buffer the vertex data from the mesh into the currently bound vertex buffer
			glBufferData(GL_ARRAY_BUFFER, toAdd.numVertices * sizeof(Vertex), toAdd.vertices, drawtype);
			//Buffer the indices used for rendering
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, toAdd.numIndices * sizeof(uint32_t), toAdd.indices, drawtype);
		}

		//Uploads new image data to the renderer
		void RenderBackendOpenGL::addImageData(Image& toAdd)
		{
			images.storedImages.push_back(OpenGL::createNewTexture(static_cast<uint16_t>(toAdd.getWidth()), static_cast<uint16_t>(toAdd.getHeight()), toAdd.getData()));
			toAdd.setID(static_cast<uint32_t>(images.storedImages.size() - 1));
		}

		//Add terrain data to the render backend, using specialized buffers
		void RenderBackendOpenGL::addTerrainData(Terrain& toAdd)
		{
			//Regenerate all the terrain buffers
			buffers.terrainvbo = OpenGL::createNewVertexBuffer();
			buffers.terrainvao = OpenGL::createNewVertexArrayObject();
			buffers.terrainabo = OpenGL::createNewIndexBuffer();

			//Load the terrain data into the correct buffers
			OpenGL::bindBuffers(buffers.terrainvbo, buffers.terrainvao, buffers.terrainabo);
			OpenGL::setupOpenGLVertexLayout();

			//Terrain is always static drawn
			for (unsigned int i = 0; i < toAdd.getNumChunks(); i++)
			{
				glBufferData(GL_ARRAY_BUFFER, toAdd.getNumVerticesPerChunk() * sizeof(Vertex), (toAdd.getData() + i)->vertices, GL_STATIC_DRAW);
			}

			for (unsigned int i = 0; i < toAdd.getNumChunks(); i++)
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, toAdd.getNumIndicesPerChunk() * sizeof(uint32_t), (toAdd.getData() + i)->indices, GL_STATIC_DRAW);
			}

			pipeline.numTerrainIndices = toAdd.getNumIndices();
			pipeline.numTerrainVertices = toAdd.getNumVertices();
			pipeline.terrainExists = true;
		}

		//Update the structure sent to the vertex shader with new world, view and projection matrices
		void RenderBackendOpenGL::updateMatrices(const glm::mat4& world, const glm::mat4& view, const glm::mat4& projection)
		{
			updateWorldMatrix(world);
			updateViewMatrix(view);
			updateProjectionMatrix(projection);
		}

		//Update only the world matrix. This should usually be done once per object
		void RenderBackendOpenGL::updateWorldMatrix(const glm::mat4& world)
		{
			storedMatrices[0] = world;
			storedMatrices[0].update();
			glUseProgram(shaders.openglShaderPrograms[0].program);
		}

		//Update only the view matrix. This only has to be done once per frame
		void RenderBackendOpenGL::updateViewMatrix(const glm::mat4& view)
		{
			storedMatrices[1] = view;
			storedMatrices[1].update();
			glUseProgram(shaders.openglShaderPrograms[0].program);
		}

		//Update only the projection matrix. This only has to be done when camera properties change or when the window is resized
		void RenderBackendOpenGL::updateProjectionMatrix(const glm::mat4& projection)
		{
			storedMatrices[2] = projection;
			storedMatrices[2].update();
			glUseProgram(shaders.openglShaderPrograms[0].program);
		}

		const uint32_t& RenderBackendOpenGL::getNextFreeImageIndex() const
		{
			return images.nextFreeImageIndex;
		}

		/*Sets the multisample level used for the rendered framebuffer*/
		void RenderBackendOpenGL::setMultisampleLevel(short toSet)
		{
			multisampleLevel = toSet;
		}

		RenderBackendOpenGL::RenderBackendOpenGL(const WindowInfo& windowInfoToSet, const std::vector<Shader>& shadersToSet) : targetWindowInfo(const_cast<WindowInfo&>(windowInfoToSet)), shaderObjects(const_cast<std::vector<Shader>&>(shadersToSet))
		{
			storedMatrices[0] = glm::mat4();
			storedMatrices[0].setName("world");
			storedMatrices[1] = glm::mat4();
			storedMatrices[1].setName("view");
			storedMatrices[2] = glm::mat4();
			storedMatrices[2].setName("projection");
			OpenGL::Defaults(&storedMatrices[0], &storedMatrices[1], &storedMatrices[2]);
			models = nullptr;
		}

		RenderBackendOpenGL::~RenderBackendOpenGL()
		{
		}

		/*Draws a single model to the screen*/
		void RenderBackendOpenGL::drawModel(Model& toDraw)
		{
			updateWorldMatrix(toDraw.transform.getTransformMatrix());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, images.storedImages[toDraw.material.albedo.getID()]);
			glDrawElements(GL_TRIANGLES, toDraw.mesh.numIndices, GL_UNSIGNED_INT, 0);
		}
	}
}
#endif