#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanStructs.h"
#include"VulkanQueue.h"
#include"VulkanSwapchain.h"
#include<set>

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Pick the best physical device to render on and get information about physical devices in the system
			void initializePhysicalDevices(Devices& devices, SwapChain& swapChain, VulkanInstance& vulkanInstance);
			//Create the vulkan logical device based on the best phycical device in the system and the supported queue families
			void createLogicalDevice(Devices& devices, SwapChain& swapChain, Pipeline& pipeline);
			//Check for the required extensions on the device
			bool checkDeviceExtensionSupport(Devices& devices, VkPhysicalDevice& deviceToCheck);
			//Rate the device based on how powerful it is and whether it supports essential features
			int deviceRating(Devices& devices, VkPhysicalDevice& device, SwapChain& swapChain);
			//Go through the devices in the system and prioritize them based on their rating
			void prioritizeDevices(Devices& devices, SwapChain& swapChain);
		}
	}
}
#endif