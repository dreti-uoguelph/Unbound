#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"Window.h"
#include<array>
#include"VulkanPipeline.h"
#include"VulkanQueue.h"
#include"VulkanImage.h"
#include"VulkanBuffer.h"
#include"VulkanRenderPass.h"
#include"VulkanStructs.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Find the swap chain capabilities associated with this device
			SwapChainProperties findSwapChainProperties(VkPhysicalDevice& physicalDevice, SwapChain& swapChain);
			//Select the best available swap surface format
			VkSurfaceFormatKHR selectSwapSurfaceFormat(SwapChain& swapChain);
			//Select the best available swap mode for the swap chain
			VkPresentModeKHR selectSwapPresentMode(SwapChain& swapChain);
			//Select the extent of the present surface
			VkExtent2D selectSwapExtent(SwapChain& swapChain, WindowInfo& targetWindowInfo);
			//Create frame buffers for the images in the swap chain
			void createFrameBuffers(Devices& devices, SwapChain& swapChain, Pipeline& pipeline);
			//Create the swap chain for displaying things to the window
			void createSwapChain(Devices& devices, SwapChain& swapChain, WindowInfo& windowInfo);
			//Recreate the swap chain, used for when the window changes
			void recreateSwapChain(Devices& devices, Buffers& buffers, SwapChain& swapChain, Pipeline& pipeline, WindowInfo& windowInfo);
			//Get image views for all the images in the swap chain
			void createSwapChainImageViews(Devices& devices, SwapChain& swapChain);
			//Clean up any resources associated with the swap chain, for swap chain recreation
			void cleanupSwapchain(Devices& devices, SwapChain& swapChain, Pipeline& pipeline);
		}
	}
}
#endif