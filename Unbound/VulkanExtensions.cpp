#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanExtensions.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Get the names of the vulkan extensions required
			std::vector<const char*> getRequiredExtensions(VulkanDebug& debug)
			{
				//Get all the glfw required extensions
				uint32_t glfwExtensionCount = 0;
				const char** glfwExtensions;
				glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

				std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

				//If validation layers are enabled, also add the extension required for that
				if (debug.enableValidationLayers)
				{
					extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
				}

				return extensions;
			}
		}
	}
}
#endif