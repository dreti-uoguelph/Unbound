#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanStructs.h"
#include"VulkanDebug.h"
#include"VulkanExtensions.h"
#include"Window.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a new vulkan instance, done before any other graphics operations
			void createVulkanInstance(Devices& devices, VulkanInstance& vulkanInstance, VulkanDebug& debug, WindowInfo& targetWindowInfo);
		}
	}
}
#endif