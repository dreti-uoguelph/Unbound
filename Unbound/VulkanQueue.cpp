#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanQueue.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Find the queue families supported by a physical device
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device, SwapChain& swapChain)
			{
				QueueFamilyIndices toReturn;
				uint32_t queueFamilyCount = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

				auto queueFamilyProperties = std::vector<VkQueueFamilyProperties>(queueFamilyCount);

				vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

				int i = 0;
				for (auto j : queueFamilyProperties)
				{
					VkBool32 presentSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, swapChain.renderSurface, &presentSupport);

					if (j.queueCount > 0 && j.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					{
						toReturn.graphicsFamily = i;
					}

					if (presentSupport)
					{
						toReturn.presentFamily = i;
					}

					if (toReturn.isComplete())
					{
						break;
					}

					i++;
				}

				return toReturn;
			}
		}
	}
}
#endif