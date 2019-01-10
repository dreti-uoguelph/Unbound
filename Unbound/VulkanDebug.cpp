#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanDebug.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Setup the callback for vulkan debugging
			void setupDebugCallback(VulkanInstance& vulkanInstance, VulkanDebug& debug)
			{
				if (!debug.enableValidationLayers)
				{
					return;
				}

				VkDebugReportCallbackCreateInfoEXT createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
				createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
				createInfo.pfnCallback = debugCallback;

				if (CreateDebugReportCallbackEXT(vulkanInstance.instance, &createInfo, nullptr, &debug.debugCallbackHandle) != VK_SUCCESS)
				{
					throw std::runtime_error("failed to set up debug callback");
				}
			}

			/*Check which validation layers are supported for debugging*/
			bool checkValidationLayerSupport(VulkanDebug& debug)
			{
				uint32_t layerCount = 0;
				vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

				std::vector<VkLayerProperties> availableLayers(layerCount);
				vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

				//Go through all the requested validation layers. If one is not found in the list of available layers, return false
				for (auto i : debug.validationLayersToUse)
				{
					bool layerFound = false;
					for (auto j : availableLayers)
					{
						if (strcmp(i, j.layerName) == 0)
						{
							layerFound = true;
							break;
						}
					}

					if (!layerFound)
					{
						return false;
					}
				}

				return true;
			}

			//Debug callback to handle vulkan errors caught by the validation layer
			static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
			{
				Logger::logWarning(std::string(msg));
				return VK_FALSE;
			}

			VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
			{
				auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

				if (func != nullptr)
				{
					return func(instance, pCreateInfo, pAllocator, pCallback);
				}
				else
				{
					return VK_ERROR_EXTENSION_NOT_PRESENT;
				}
			}

			void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
			{
				auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

				if (func != nullptr)
				{
					func(instance, callback, pAllocator);
				}
			}
		}
	}
}
#endif