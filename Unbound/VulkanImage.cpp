#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanImage.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a vulkan image using the specified parameters
			void createImage(Devices& devices, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags properties, VkImage& outImage, VkDeviceMemory& outMemory)
			{
				VkImageCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				createInfo.extent.width = width;
				createInfo.extent.height = height;
				createInfo.extent.depth = 1;
				createInfo.mipLevels = 1;
				createInfo.arrayLayers = 1;
				createInfo.format = format;
				createInfo.tiling = tiling;
				createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				createInfo.usage = usageFlags;
				createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				createInfo.imageType = VK_IMAGE_TYPE_2D;

				if (vkCreateImage(devices.device, &createInfo, nullptr, &outImage) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create vulkan image for rendering");
				}

				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(devices.device, outImage, &memRequirements);

				VkMemoryAllocateInfo allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memRequirements.size;
				allocInfo.memoryTypeIndex = findMemoryType(devices, memRequirements.memoryTypeBits, properties);

				if (vkAllocateMemory(devices.device, &allocInfo, nullptr, &outMemory) != VK_SUCCESS)
				{
					Logger::log("Error: could not allocate memory for creating a vulkan image");
					throw std::runtime_error("Could not allocate memory for creating vulkan image");
				}

				vkBindImageMemory(devices.device, outImage, outMemory, 0);
			}

			//Create an image view into a given, already initialized image
			VkImageView createImageView(Devices& devices, VkImage image, VkFormat imageFormat, VkImageAspectFlags aspectFlags)
			{
				VkImageViewCreateInfo imageViewCreateInfo = {};
				imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageViewCreateInfo.image = image;
				imageViewCreateInfo.format = imageFormat;
				imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
				imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
				imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
				imageViewCreateInfo.subresourceRange.levelCount = 1;
				imageViewCreateInfo.subresourceRange.layerCount = 1;

				VkImageView imageview;
				if (vkCreateImageView(devices.device, &imageViewCreateInfo, nullptr, &imageview) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create image view into already existing image");
				}

				return imageview;
			}

			//Find the best supported image format that matches the selected requirements
			VkFormat findBestSupportedImageFormat(Devices& devices, std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
			{
				for (auto i : candidates)
				{
					VkFormatProperties properties;
					vkGetPhysicalDeviceFormatProperties(devices.physicalDevice, i, &properties);

					if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
					{
						return i;
					}
					else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)
					{
						return i;
					}
				}

				throw std::runtime_error("Failed to find supported image format");
			}

			//Create the depth buffer and related resources
			void createDepthBuffer(Devices& devices, Buffers& buffers, SwapChain& swapChain, Pipeline& pipeline)
			{
				swapChain.depthFormat = findDepthFormat(devices);

				uint32_t depthBufferWidth = swapChain.extent.width;
				uint32_t depthBufferHeight = swapChain.extent.height;

				createImage(devices, depthBufferWidth, depthBufferHeight, swapChain.depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, swapChain.depthBufferImage, swapChain.depthBufferMemory);
				swapChain.depthBufferImageView = createImageView(devices, swapChain.depthBufferImage, swapChain.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

				transitionImageLayout(devices, buffers, swapChain, pipeline, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, swapChain.depthBufferImage, swapChain.depthFormat);
			}

			VkFormat findDepthFormat(Devices& devices)
			{
				std::vector<VkFormat> formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
				return findBestSupportedImageFormat(devices, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
			}

			//Returns a bool indicating whether a depth image format has a stencil component
			bool depthImageHasStencilComponent(VkFormat format)
			{
				return format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
			}

			//Transition the layout of an already existing image
			void transitionImageLayout(Devices& devices, Buffers& buffers, SwapChain& swapChain, Pipeline& pipeline, VkImageLayout oldLayout, VkImageLayout newLayout, VkImage& image, VkFormat imageFormat)
			{
				VkPipelineStageFlags sourceStage, dstStage;

				VkImageMemoryBarrier barrier = {};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.oldLayout = oldLayout;
				barrier.newLayout = newLayout;
				barrier.image = image;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.layerCount = 1;
				barrier.subresourceRange.levelCount = 1;

				//Set the correct aspect mask if the new layout is a depth image
				if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
				{
					barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

					if (depthImageHasStencilComponent(imageFormat))
					{
						barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
					}
				}

				//Set the correct access masks and pipeline stages depending on the old and new layouts
				if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
				{
					barrier.srcAccessMask = 0;
					barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				}
				else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
				{
					barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
					sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
					dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				}
				else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
				{
					barrier.srcAccessMask = 0;
					barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				}
				else
				{
					throw std::invalid_argument("Unsupported image layout transition used");
				}

				Vulkan::beginSingleSubmitCommands(devices, buffers, pipeline.copyCommandPool);
				vkCmdPipelineBarrier(buffers.singleSubmitBuffer, sourceStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
				Vulkan::endSingleSubmitCommands(devices, pipeline, buffers.singleSubmitBuffer, pipeline.copyCommandPool);
			}

			//Create a texture image with reasonable default properties
			void createTextureImage(Devices& devices, SwapChain& swapChain, Pipeline& pipeline, Buffers& buffers, Image& toCreateFrom, VkImage& imageOut, VkDeviceMemory& deviceMemoryOut)
			{
				createImage(devices, toCreateFrom.getWidth(), toCreateFrom.getHeight(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageOut, deviceMemoryOut);
				transitionImageLayout(devices, buffers, swapChain, pipeline, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, imageOut, VK_FORMAT_R8G8B8A8_UNORM);

				//Copy the image into the staging buffer
				memcpy(MemoryManager::get()->textureStagingPool.addData(toCreateFrom.getSizeInBytes()), toCreateFrom.getData(), toCreateFrom.getSizeInBytes());

				copyBufferToImage(devices, buffers, pipeline, buffers.textureStagingBuffer, imageOut, toCreateFrom.getWidth(), toCreateFrom.getHeight());
				transitionImageLayout(devices, buffers, swapChain, pipeline, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, buffers.textureImages[buffers.nextFreeImageIndex], VK_FORMAT_R8G8B8A8_UNORM);
				buffers.textureImageViews[buffers.nextFreeImageIndex] = Vulkan::createImageView(devices, buffers.textureImages[buffers.nextFreeImageIndex], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
			}

			//Creates the texture samplers that will be used in the scene
			void createTextureSamplers(Devices& devices, Pipeline& pipeline)
			{
				//Also create the default sampler, which will be used for most textures
				VkSamplerCreateInfo samplerCreateInfo = {};
				samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
				samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
				samplerCreateInfo.anisotropyEnable = VK_TRUE;
				samplerCreateInfo.maxAnisotropy = 16;
				samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
				samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

				samplerCreateInfo.compareEnable = VK_FALSE;
				samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;

				samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				samplerCreateInfo.mipLodBias = 0.0f;
				samplerCreateInfo.minLod = 0.0f;
				samplerCreateInfo.maxLod = 0.0f;

				if (vkCreateSampler(devices.device, &samplerCreateInfo, nullptr, &pipeline.textureSampler) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create texture sampler for texture rendering");
				}
			}
		}
	}
}
#endif