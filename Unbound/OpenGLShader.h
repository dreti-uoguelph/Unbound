#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_OPENGL
#include"OpenGLStructs.h"
#include"Shader.h"
#include"glm\gtc\type_ptr.hpp"
#include<tuple>

namespace Unbound
{
	namespace Graphics
	{
		namespace OpenGL
		{
			struct Defaults
			{
				static UniformBuffer worldViewProjection;
				static UniformBuffer staticUniformBuffer, defaultUniformBuffer, dynamicUniformBuffer;

				Defaults(ShaderObserver<glm::mat4>* world, ShaderObserver<glm::mat4>* view, ShaderObserver<glm::mat4>* projection);
			};

			/*Create and compile a new opengl shader from an abstract shader object*/
			const OpenGLShaderBackend createNewShader(const Shader& toCompileFrom);

			/*Create and compile a shader, and link the uniforms to the corrosponding input values*/
			const OpenGLShaderBackend createNewShader(const Shader& toCompileFrom, const std::vector<UniformBuffer*>& boundInputs);

			/*Create and link a new shader program from two existing shaders*/
			const ShaderProgram createNewShaderProgram(Shaders& shaders, const OpenGLShaderBackend& vertex, const OpenGLShaderBackend& fragment);

			/*Release all shaders that are currently active*/
			void releaseAllShaders(const std::vector<OpenGLShaderBackend>& shadersToRelease);
		}
	}
}
#endif