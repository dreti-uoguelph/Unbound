#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"glm\gtc\matrix_transform.hpp"
#include<unordered_map>

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			/*Holds the main buffers used throughout the renderer*/
			struct Buffers
			{
				GLuint vertexBufferObject; //Vertex buffer for sending vertex data to the graphics card
				GLuint vertexArrayObject; //buffer to store vertex attribute arrays
				GLuint indexBufferObject; //Buffer to store indices used for rendering

				GLuint terrainvbo, terrainvao, terrainabo; //Buffers to hold terrain data, might make certain optimizations easier in the future
				GLuint postprocessvbo, postprocessvao, postprocessibo; //Buffer to hold the vertex data for elements that are to be drawn on top of the scene
				GLuint postprocessIntermediateFramebuffer; //An intermediate framebuffer used for multisampled rendering
				GLuint postProcessFrameBuffer; //A frame buffer that will be rendered to before post processing

				static std::vector<int> ints;

				Buffers() noexcept
				{
					postprocessIntermediateFramebuffer = 0;
					vertexBufferObject = 0;
					vertexArrayObject = 0;
					indexBufferObject = 0;
					terrainabo = 0;
					terrainvao = 0;
					terrainvbo = 0;
					postprocessvbo = 0;
					postprocessvao = 0;
					postprocessibo = 0;
					postProcessFrameBuffer = 0;
				}
			};

			/*Holds data related to image handling and rendering*/
			struct Images
			{
				uint32_t nextFreeImageIndex; //The index of the next free image slot in the render
				std::vector<GLuint> storedImages; //The images loaded into the renderer
				GLuint renderedOutput; //Pipeline images, which are kept separately for convenience
				GLuint blankImage; //A blank image used internally by the renderer in several places. Point is: it's important

				Images() noexcept
				{
					renderedOutput = 0;
					blankImage = 0;
					nextFreeImageIndex = 0;
					storedImages.reserve(4096);
					storedImages.push_back(0);
				}
			};

			class UniformBuffer;

			/*Observes a shader variable change and updates all the uniform buffers that it is part of*/
			template<typename T>
			class ShaderObserver
			{
			public:
				/*Updates all uniform buffers with the updated variable data*/
				inline void update()
				{
					for (auto i : attachedBuffers)
					{
						i->updateBuffer();
					}
				}

				/*Changes the variable being observed to a new value*/
				inline void update(const T& newTarget)
				{
					internalVar = newTarget;
					update();
				}

				/*Attaches a shader backend to update when the internal variable changes*/
				inline void attach(UniformBuffer* toAttach)
				{
					attachedBuffers.push_back(toAttach);
				}

				/*Sets the name of the variable being observed*/
				inline void setName(const std::string& name)
				{
					this->name = name;
				}

				/*Detaches a shader program from the observer, which will stop it from being updated*/
				inline void detach(UniformBuffer* toDetach)
				{
					uint32_t j = 0;
					for (auto i : attachedBuffers)
					{
						if (i == toDetach)
						{
							attachedBuffers.erase(attachedBuffers.begin() + j);
						}

						j++;
					}
				}

				/*Returns a pointer to the internal variable to work with*/
				inline T* var() const
				{
					return const_cast<T*>(&internalVar);
				}

				/*Returns the name of the variable being observed*/
				inline const std::string& getName() const
				{
					return name;
				}

				inline void operator=(const T& other)
				{
					update(other);
				}

				ShaderObserver()
				{
					attachedBuffers.reserve(64);
				}

				ShaderObserver(const T& initial)
				{
					internalVar = initial;
					attachedBuffers.reserve(64);
				}

				~ShaderObserver()
				{

				}
			private:
				T internalVar; //The variable being observed
				std::string name; //The name of the variable being observed
				std::vector<UniformBuffer*> attachedBuffers; //A vector of uniform buffers to be updated
			};

			/*Represents a uniform buffer, with its attached shader inputs*/
			class UniformBuffer
			{
			public:

				/*Initialize the uniform buffer to be a new empty buffer*/
				inline void init()
				{
					glGenBuffers(1, &uboHandle);
				}

				/*Binds a new shader input to be sent in a uniform buffer into the shader*/
				inline void bindInput(ShaderObserver<GLuint>* ptInput)
				{
					bindInput(ptInput->var(), sizeof(GLuint), 1);
				}

				/*Binds a new shader input to be sent in a uniform buffer into the shader*/
				inline void bindInput(ShaderObserver<GLfloat>* ptInput)
				{
					bindInput(ptInput->var(), sizeof(GLuint), 1);
				}

				/*Binds a new shader input to be sent in a uniform buffer into the shader*/
				inline void bindInput(ShaderObserver<glm::vec3>* ptInput)
				{
					bindInput(ptInput->var(), sizeof(glm::vec3), 16);
				}

				/*Binds a new shader input to be sent in a uniform buffer into the shader*/
				inline void bindInput(ShaderObserver<glm::vec4>* ptInput)
				{
					bindInput(ptInput->var(), sizeof(glm::vec4), 16);
				}

				/*Binds a new shader input to be sent in a uniform buffer into the shader*/
				inline void bindInput(ShaderObserver<glm::mat4>* ptInput)
				{
					bindInput(ptInput->var(), sizeof(glm::mat4), 64);
				}

				/*Updates the buffer with new data*/
				inline void updateBuffer()
				{
					glBindBuffer(GL_UNIFORM_BUFFER, uboHandle);

					//Copy all the data into the internal buffer. Note: this is terrible!
					for (uint32_t i = 0; i < variableAlignments.size(); ++i)
					{
						memcpy(data.data() + variableAlignments[i], variablePointers[i], variableSizes[i]);
					}

					glBufferData(GL_UNIFORM_BUFFER, totalSize, data.data(), GL_STREAM_DRAW);
					glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboHandle);
				}

				/*Sets the name of the uniform buffer*/
				inline void setName(const std::string& name)
				{
					this->name = name;
				}

				/*Returns the name of the uniform buffer*/
				const std::string& getName() const
				{
					return name;
				}

				UniformBuffer() noexcept
				{
					uboHandle = 0;
					data.resize(8096, 0); //Default size of uniform buffer is 8kb
					variableSizes.reserve(8);
					variableAlignments.reserve(8);
					variablePointers.reserve(8);
					name = "";
				}

				UniformBuffer(const std::string& name)
				{
					UniformBuffer();
					this->name = name;
				}

				~UniformBuffer()
				{
					glDeleteBuffers(1, &uboHandle);
				}

			private:
				GLuint uboHandle; //Uniform buffer object for sending shader inputs
				GLuint uboBindingPoint; //The binding point for the ubo
				std::string name; //The name of the uniform buffer
				uint32_t totalSize; //The total size of the input buffer

				std::vector<uint32_t> variableSizes; //Stores the size of each variable in the buffer
				std::vector<uint32_t> variableAlignments; //Stores the alignment of each variable in the buffer
				std::vector<void*> variablePointers; //Pointers to the raw variable values
				std::vector<char> data; //Stores the raw data within the buffer

				/*Binds a new shader input to be sent in a uniform buffer into the shader*/
				inline void bindInput(void* ptInput, uint32_t sizeOfInput, uint8_t inputAlignment)
				{
					uint32_t alignment = 0;

					if(variableSizes.size() > 0)
						alignment = variableAlignments.back() + variableSizes.back();

					//Make sure the alignment is at the right multiple of the variable size
					if (alignment % inputAlignment > 0)
					{
						if (alignment > 0)
							alignment = (alignment - (alignment % inputAlignment)) + inputAlignment;
					}
					
					//If the size of the buffer is less than the target alignment, allocate more memory
					if (data.size() < alignment)
					{
						data.resize(data.size() * 2, 0);
					}

					variableSizes.push_back(sizeOfInput);
					variableAlignments.push_back(alignment);
					variablePointers.push_back(ptInput);
					totalSize = alignment + sizeOfInput;
					memcpy_s(data.data() + alignment, data.size(), ptInput, sizeOfInput);
				}
			};

			/*Holds info about opengl specific shader data such as handles*/
			class OpenGLShaderBackend
			{
			public:
				GLuint shaderObjectHandle; //Handle to the opengl shader object

				OpenGLShaderBackend() noexcept
				{
					shaderObjectHandle = 0;
					
				}

				OpenGLShaderBackend(const GLuint handleToSet) : shaderObjectHandle(handleToSet)
				{
					
				}

				~OpenGLShaderBackend()
				{
					
				}
			};

			/*Holds a set of info about individual shader programs, such as uniform locations and components*/
			struct ShaderProgram
			{
				OpenGLShaderBackend vertex; //The vertex shader backend
				OpenGLShaderBackend fragment; //The fragment shader backend
				GLuint program; //The handle to the opengl shader program

				/*Attaches a new uniform buffer to the shader backend*/
				inline void attach(UniformBuffer* toAttach)
				{
					attachedUniformBuffers.push_back(toAttach);

					uniformBufferIndices.push_back(glGetUniformBlockIndex(program, toAttach->getName().c_str()));
				}

				ShaderProgram() noexcept
				{
					attachedUniformBuffers.reserve(8);
				}

				~ShaderProgram()
				{
					
				}

			private:

				std::vector<UniformBuffer*> attachedUniformBuffers; //The uniform buffers that are part of the shader
				std::vector<GLuint> uniformBufferIndices; //The indices of the various attached uniform buffers in the shader
			};

			/*Holds data relating to shaders stored in the renderer*/
			struct Shaders
			{
				GLuint uniformBindingIndex; //The index of the last ubo binding point
				std::vector<OpenGLShaderBackend> storedShaderObjects; //Handles to the individial compiled shader objects in the renderer
				std::vector<ShaderProgram> openglShaderPrograms; //All the shader programs currently in the renderer

				Shaders() noexcept
				{
					uniformBindingIndex = 0;
					storedShaderObjects.reserve(256);
					openglShaderPrograms.reserve(128);
				}
			};

			/*Holds data relating to the opengl rendering pipeline*/
			struct Pipeline
			{
				bool terrainExists; //Whether there is a terrain currently in the scene
				uint32_t numTerrainVertices, numTerrainIndices; //The number of vertices and indices in the main terrain

				Pipeline() noexcept
				{
					numTerrainVertices = 0;
					numTerrainIndices = 0;
					terrainExists = false;
				}
			};
		}
	}
}
#endif