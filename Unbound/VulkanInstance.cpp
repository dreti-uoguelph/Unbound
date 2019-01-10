#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanInstance.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a new vulkan instance, done before any other graphics operations
			void createVulkanInstance(Devices& devices, VulkanInstance& vulkanInstance, VulkanDebug& debug, WindowInfo& targetWindowInfo)
			{
				debug.validationLayersToUse.push_back("VK_LAYER_LUNARG_standard_validation");

				//Check for validation layers
				if (debug.enableValidationLayers && !checkValidationLayerSupport(debug))
				{
					throw std::runtime_error("Vulkan validation layers not available");
				}

				//Initialize the appInfo struct with common information about this program
				VkApplicationInfo appInfo = {};
				appInfo.apiVersion = VK_API_VERSION_1_1;
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.pApplicationName = targetWindowInfo.windowName.c_str();
				appInfo.engineVersion = VK_MAKE_VERSION(UNBOUND_VERSION_MAJOR, UNBOUND_VERSION_MINOR, UNBOUND_VERSION_PATCH);
				appInfo.pEngineName = "Unbound";
				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

				VkInstanceCreateInfo instanceCreateInfo = {};
				instanceCreateInfo.pApplicationInfo = &appInfo;
				instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

				//Get the required extensions for vulkan
				auto extensionNames = getRequiredExtensions(debug);
				instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();
				instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());

				if (debug.enableValidationLayers)
				{
					instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(debug.validationLayersToUse.size());
					instanceCreateInfo.ppEnabledLayerNames = debug.validationLayersToUse.data();
				}

				//Create the vulkan instance
				if (vkCreateInstance(&instanceCreateInfo, nullptr, &vulkanInstance.instance) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create vulkan instance\nCheck hardware and drivers!");
				}

				//Get the properties of extensions supported by vulkan
				uint32_t vulkanExtensionCount = 0;

				vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, nullptr);
				devices.vulkanExtensions = std::vector<VkExtensionProperties>(vulkanExtensionCount);
				vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, devices.vulkanExtensions.data());
			}
		}
	}
}
#endif