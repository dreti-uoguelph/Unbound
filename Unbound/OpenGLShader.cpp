#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLShader.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			UniformBuffer Defaults::worldViewProjection = UniformBuffer();
			UniformBuffer Defaults::staticUniformBuffer = UniformBuffer();
			UniformBuffer Defaults::defaultUniformBuffer = UniformBuffer();
			UniformBuffer Defaults::dynamicUniformBuffer = UniformBuffer();

			Defaults::Defaults(ShaderObserver<glm::mat4>* world, ShaderObserver<glm::mat4>* view, ShaderObserver<glm::mat4>* projection)
			{
				if (worldViewProjection.getName() == "")
				{
					worldViewProjection.setName("worldViewProjection");
					world->attach(&Defaults::worldViewProjection);
					Defaults::worldViewProjection.bindInput(world);
					view->attach(&Defaults::worldViewProjection);
					Defaults::worldViewProjection.bindInput(view);
					projection->attach(&Defaults::worldViewProjection);
					Defaults::worldViewProjection.bindInput(projection);
				}
			}

			/*Create and compile a new opengl shader from an abstract shader object*/
			const OpenGLShaderBackend createNewShader(const Shader& toCompileFrom)
			{
				Logger::log("Compiling shader: " + toCompileFrom.name);
				GLenum shaderType;

				switch (toCompileFrom.type)
				{
				case SHADER_TYPE_VERTEX:
					shaderType = GL_VERTEX_SHADER;
					break;
				case SHADER_TYPE_FRAGMENT:
					shaderType = GL_FRAGMENT_SHADER;
					break;
				case SHADER_TYPE_COMPUTE:
					shaderType = GL_COMPUTE_SHADER;
					break;
				default:
					shaderType = 0;
					break;
				}

				//Create the shader
				GLuint shader = glCreateShader(shaderType);

				auto shaderSourceData = const_cast<const GLchar*>(const_cast<Shader&>(toCompileFrom).getBuffer().data());
				GLint length = static_cast<GLint>(const_cast<Shader&>(toCompileFrom).getBuffer().size());
				GLint* lengthp = &length;

				//Attach the shader source from the shader's buffer and compile the shader
				glShaderSource(shader, 1, &shaderSourceData, const_cast<const GLint*>(lengthp));
				glCompileShader(shader);

				//Check for any shader compilation errors and display them
				GLint compileStatus;
				GLchar buffer[512];

				glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

				if (!compileStatus)
				{
					glGetShaderInfoLog(shader, 512, 0, buffer);
					std::string toWrite = std::string(buffer);
					toWrite = toWrite.substr(0, toWrite.length() - 1);

					Logger::logError("Shader compilation error: " + toWrite);
					return OpenGLShaderBackend();
				}

				return OpenGLShaderBackend(shader);
			}

			/*Create and compile a shader, and link the uniforms to the corrosponding input values*/
			const OpenGLShaderBackend createNewShader(const Shader& toCompileFrom, const std::vector<UniformBuffer*>& boundInputs)
			{
				auto toReturn = createNewShader(toCompileFrom);
				return toReturn;
			}

			/*Create and link a new shader program from two existing shaders*/
			const ShaderProgram createNewShaderProgram(Shaders& shaders, const OpenGLShaderBackend& vertex, const OpenGLShaderBackend& fragment)
			{
				Logger::log(std::string("Linking shader program from shaders ") + std::to_string(static_cast<unsigned int>(vertex.shaderObjectHandle)) + " and " + std::to_string(fragment.shaderObjectHandle));

				//Create the shader program
				ShaderProgram toReturn;
				toReturn.program = glCreateProgram();
				toReturn.vertex = OpenGLShaderBackend(vertex);
				toReturn.fragment = OpenGLShaderBackend(fragment);

				//Attach both the vertex and fragment shaders to the program
				glAttachShader(toReturn.program, vertex.shaderObjectHandle);
				glAttachShader(toReturn.program, fragment.shaderObjectHandle);

				//Link the program
				glLinkProgram(toReturn.program);

				//Check for and report any errors
				GLint linkStatus;
				GLchar buffer[512];
				glGetProgramiv(toReturn.program, GL_LINK_STATUS, &linkStatus);

				if (!linkStatus)
				{
					glGetProgramInfoLog(toReturn.program, 512, 0, buffer);
					std::string toWrite = std::string(buffer);
					toWrite = toWrite.substr(0, toWrite.length() - 1);

					Logger::logError("Shader program linking error: " + toWrite);
					return ShaderProgram();
				}
				
				//Add the default uniform buffers to the shader
				toReturn.attach(&Defaults::worldViewProjection);
				glUseProgram(toReturn.program);
				glUniformBlockBinding(toReturn.program, 0, 0);

				return toReturn;
			}

			/*Release all shaders that are currently active*/
			void releaseAllShaders(const std::vector<OpenGLShaderBackend>& shadersToRelease)
			{
				for (auto i : shadersToRelease)
				{
					glDeleteShader(i.shaderObjectHandle);
				}
			}
		}
	}
}
#endif