#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanStructs.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Get the names of the vulkan extensions required
			std::vector<const char*> getRequiredExtensions(VulkanDebug& debug);
		}
	}
}
#endif