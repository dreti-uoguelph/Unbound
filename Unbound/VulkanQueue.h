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
			//Find the queue families supported by a physical device
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device, SwapChain& swapChain);
		}
	}
}
#endif