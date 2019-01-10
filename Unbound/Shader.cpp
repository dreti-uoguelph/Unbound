#include "Shader.h"
#include"stdafx.h"
#include"Shader.h"

namespace Unbound
{
	namespace Graphics
	{
		void Shader::load(std::string filePath)
		{
			std::ifstream inFile;
			size_t fileSize = 0;

#ifdef UNBOUND_BACKEND_VULKAN
			inFile.open(filePath, std::ios::ate | std::ios::binary);
#else
			inFile.open(filePath, std::ios::ate);
#endif

			if (!inFile.is_open())
			{
				Logger::log("Error: could not open shader file: " + filePath);
			}

			fileSize = (size_t)inFile.tellg();
			buffer.resize(fileSize);
			inFile.seekg(0);
			inFile.read(buffer.data(), fileSize);
			inFile.close();
		}

		void Shader::unload()
		{
		}

		const std::vector<char>& Shader::getBuffer()
		{
			return buffer;
		}

		Shader::Shader() noexcept
		{
			type = ShaderType::SHADER_TYPE_VERTEX;
		}

		Shader::Shader(const std::string& path) noexcept
		{
			name = path;
			type = ShaderType::SHADER_TYPE_VERTEX;

			//Try to automatically find the shader type
			if (path.substr(path.find_last_of('.')) == "vert")
			{
				type = ShaderType::SHADER_TYPE_VERTEX;
			}
			else if (path.substr(path.find_last_of('.')) == "frag")
			{
				type = ShaderType::SHADER_TYPE_FRAGMENT;
			}

			load(path);
		}

		Shader::Shader(const std::string& path, ShaderType typeToSet)
		{
			name = path;
			type = typeToSet;
			load(path);
		}

		Shader::~Shader()
		{
		}
	}
}