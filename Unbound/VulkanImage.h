#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanMemory.h"
#include"VulkanBuffer.h"
#include"VulkanStructs.h"
#include"Image.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a vulkan image using the specified parameters
			void createImage(Devices& devices, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outMemory);
			//Create an image view into a given, already initialized image
			VkImageView createImageView(Devices& devices, VkImage image, VkFormat imageFormat, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
			//Returns a bool indicating whether a depth image format has a stencil component
			bool depthImageHasStencilComponent(VkFormat format);
			//Find the best supported image format that matches the selected requirements
			VkFormat findBestSupportedImageFormat(Devices& devices, std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
			//Find a suitable format for creating a depth image
			VkFormat findDepthFormat(Devices& devices);
			//Create the depth buffer and related resources
			void createDepthBuffer(Devices& devices, Buffers& buffers, SwapChain& swapChain, Pipeline& pipline);
			//Transition the layout of an already existing image
			void transitionImageLayout(Devices& devices, Buffers& buffers, SwapChain& swapChain, Pipeline& pipeline, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage& image, VkFormat imageFormat);
			//Create a texture image with reasonable default properties
			void createTextureImage(Devices& devices, SwapChain& swapChain, Pipeline& pipeline, Buffers& buffers, Image& toCreateFrom, VkImage& imageOut, VkDeviceMemory& deviceMemoryOut);
			//Creates the texture samplers that will be used in the scene
			void createTextureSamplers(Devices& devices, Pipeline& pipeline);
		}
	}
}
#endif