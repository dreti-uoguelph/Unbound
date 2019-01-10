#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanMemory.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Find the right type of memory to use when allocating memory for a buffer
			uint32_t findMemoryType(Devices& devices, uint32_t typefilter, VkMemoryPropertyFlags properties)
			{
				VkPhysicalDeviceMemoryProperties memProperties = {};
				vkGetPhysicalDeviceMemoryProperties(devices.physicalDevice, &memProperties);

				for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
				{
					if (typefilter & (1 << i))
					{
						if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)
						{
							return i;
						}
					}
				}

				throw std::runtime_error("Could not find a suitable memory type for creating a buffer");
			}
		}
	}
}
#endif