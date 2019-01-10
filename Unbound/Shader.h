#pragma once
#include "stdafx.h"
#include <fstream>

namespace Unbound
{
	namespace Graphics
	{
		/*The type of shader that a shader object represents*/
		enum ShaderType { SHADER_TYPE_VERTEX, SHADER_TYPE_FRAGMENT, SHADER_TYPE_COMPUTE };

		/*A wrapper class for storing and accessing shader data*/
		class Shader
		{
		public:
			std::string name; //The name of the shader displayed in the engine

			//Load a shader from the specified path. Should be an SPV compiled SPIR-V binary
			void load(std::string filePath);
			//Free the buffer and unload shader data
			void unload();
			//Get the text buffer of the bytecode
			const std::vector<char>& getBuffer();

			//The type of this shader
			ShaderType type;

			Shader() noexcept;
			Shader(const std::string& path) noexcept;
			Shader(const std::string& path, ShaderType typeToSet);
			~Shader();
		private:
			std::vector<char> buffer; //The contents of the compiled shader binary, in byte form
		};
	}
}