#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a new shader module from loaded bytecode
			VkShaderModule createShaderModule(VkDevice device, std::vector<char>& bytecode);
		}
	}
}
#endif