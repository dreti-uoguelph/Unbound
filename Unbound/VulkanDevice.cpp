#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanDevice.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Pick the best physical device to render on and get information about physical devices in the system
			void initializePhysicalDevices(Devices& devices, SwapChain& swapChain, VulkanInstance& vulkanInstance)
			{
				//Get the number of devices in the system
				uint32_t deviceCount;
				vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, nullptr);

				if (!deviceCount)
				{
					throw std::runtime_error("No devices capable of running vulkan were found\nPlease check devices and drivers");
				}

				//Get information about the physical devices in the system
				devices.physicalDevices = std::vector<VkPhysicalDevice>(deviceCount);
				vkEnumeratePhysicalDevices(vulkanInstance.instance, &deviceCount, devices.physicalDevices.data());

				//Prioritize devices based on rating
				prioritizeDevices(devices, swapChain);

				devices.physicalDevice = VK_NULL_HANDLE;

				if (devices.deviceRatings.rbegin()->first >= 0)
				{
					devices.physicalDevice = devices.deviceRatings.rbegin()->second;
				}
				else
				{
					throw std::runtime_error("Could not find a suitable GPU to render on");
				}
			}

			//Create the vulkan logical device based on the best phycical device in the system and the supported queue families
			void createLogicalDevice(Devices& devices, SwapChain& swapChain, Pipeline& pipeline)
			{
				//Get the queue families on the device
				Vulkan::QueueFamilyIndices indices = Vulkan::findQueueFamilies(devices.physicalDevice, swapChain);

				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

				//Create a new vulkan queue for graphics
				float queuePriority = 1.0f;
				for (int i : uniqueQueueFamilies)
				{
					VkDeviceQueueCreateInfo queueCreateInfo = {};
					queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
					queueCreateInfo.queueCount = 1;
					queueCreateInfo.pQueuePriorities = &queuePriority;
					queueCreateInfos.push_back(queueCreateInfo);
				}

				//Set up features to require needed techniques
				VkPhysicalDeviceFeatures deviceFeatures = {};
				deviceFeatures.samplerAnisotropy = VK_TRUE;

				VkDeviceCreateInfo deviceCreateInfo = {};
				deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
				deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
				deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
				deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(devices.deviceExtensionsToUse.size());
				deviceCreateInfo.ppEnabledExtensionNames = devices.deviceExtensionsToUse.data();

				if (vkCreateDevice(devices.physicalDevice, &deviceCreateInfo, nullptr, &devices.device) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create vulkan logical device\nCheck device capabilities and supported features");
				}

				vkGetDeviceQueue(devices.device, indices.graphicsFamily, 0, &pipeline.graphicsQueue);
				vkGetDeviceQueue(devices.device, indices.presentFamily, 0, &pipeline.presentQueue);
			}

			//Check for the required extensions on the device
			bool checkDeviceExtensionSupport(Devices& devices, VkPhysicalDevice& deviceToCheck)
			{
				uint32_t extensioncount = 0;
				vkEnumerateDeviceExtensionProperties(deviceToCheck, nullptr, &extensioncount, nullptr);

				std::vector<VkExtensionProperties> availableExtensions(extensioncount);
				vkEnumerateDeviceExtensionProperties(deviceToCheck, nullptr, &extensioncount, availableExtensions.data());

				//Copy the required extensions into the set and erase each supported one
				std::set<std::string> requiredExtensions(devices.deviceExtensionsToUse.begin(), devices.deviceExtensionsToUse.end());

				for (auto i : availableExtensions)
				{
					requiredExtensions.erase(i.extensionName);
				}

				//If the set is empty, all required extensions are supported. Otherwise, there are required extensions that are not supported
				return requiredExtensions.empty();
			}

			//Rate the device based on how powerful it is and whether it supports essential features
			int deviceRating(Devices& devices, VkPhysicalDevice& device, SwapChain& swapChain)
			{
				int deviceRating = 0;

				VkPhysicalDeviceProperties deviceProperties;
				vkGetPhysicalDeviceProperties(device, &deviceProperties);

				VkPhysicalDeviceFeatures deviceFeatures;
				vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

				//Increase the device rating if it is a discrete GPU, so that these are selected first
				if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					deviceRating += 10;
				}

				//The device is not suitable if it does not support geometry shaders or the right queue families
				//The device must also support all required features and extensions
				if (!deviceFeatures.geometryShader || !deviceFeatures.samplerAnisotropy || !Vulkan::findQueueFamilies(device, swapChain).isComplete() || !checkDeviceExtensionSupport(devices, device))
				{
					return 0;
				}

				//The device also needs to support at least one swapchain format and present mode
				bool swapChainAdequate = false;
				if (checkDeviceExtensionSupport(devices, device))
				{
					Vulkan::SwapChainProperties properties = Vulkan::findSwapChainProperties(device, swapChain);
					swapChainAdequate = !properties.availableFormats.empty() && !properties.presentModes.empty();
				}

				if (!swapChainAdequate)
				{
					return 0;
				}

				return deviceRating;
			}

			//Go through the devices in the system and prioritize them based on their rating
			void prioritizeDevices(Devices& devices, SwapChain& swapChain)
			{
				for (auto i : devices.physicalDevices)
				{
					devices.deviceRatings.insert(std::make_pair(deviceRating(devices, i, swapChain), i));
				}
			}
		}
	}
}
#endif