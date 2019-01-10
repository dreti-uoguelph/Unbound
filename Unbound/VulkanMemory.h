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
			//Find the right type of memory to use when allocating memory for a buffer
			uint32_t findMemoryType(Devices& devices, uint32_t typefilter, VkMemoryPropertyFlags properties);
		}
	}
}
#endif