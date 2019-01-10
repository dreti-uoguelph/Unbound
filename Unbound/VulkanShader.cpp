#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanShader.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a new vulkan shader module from loaded bytecode
			VkShaderModule createShaderModule(VkDevice device, std::vector<char>& bytecode)
			{
				VkShaderModuleCreateInfo shaderCreateInfo = {};
				shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				shaderCreateInfo.codeSize = bytecode.size();
				shaderCreateInfo.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

				VkShaderModule shaderModule;
				if (vkCreateShaderModule(device, &shaderCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create shader module from bytecode");
				}

				return shaderModule;
			}
		}
	}
}
#endif