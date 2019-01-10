#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include<array>
#include"VulkanDevice.h"
#include"VulkanSwapchain.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a render pass to describe buffers through rendering operations
			void createRenderPass(Devices& devices, SwapChain& swapChain, Pipeline& pipeline);
		}
	}
}
#endif