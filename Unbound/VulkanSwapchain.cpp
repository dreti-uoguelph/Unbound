#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanSwapchain.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Find the swap chain capabilities associated with this device
			SwapChainProperties findSwapChainProperties(VkPhysicalDevice& physicalDevice, SwapChain& swapChain)
			{
				SwapChainProperties properties;
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, swapChain.renderSurface, &properties.capabilities);

				uint32_t formatcount = 0;
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, swapChain.renderSurface, &formatcount, nullptr);

				if (formatcount)
				{
					properties.availableFormats.resize(formatcount);
					vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, swapChain.renderSurface, &formatcount, properties.availableFormats.data());
				}

				uint32_t presentmodecount = 0;
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, swapChain.renderSurface, &presentmodecount, nullptr);

				if (presentmodecount)
				{
					properties.presentModes.resize(presentmodecount);
					vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, swapChain.renderSurface, &presentmodecount, properties.presentModes.data());
				}

				return properties;
			}

			//Select the best available swap surface format
			VkSurfaceFormatKHR selectSwapSurfaceFormat(SwapChain& swapChain)
			{
				if (swapChain.properties.availableFormats.size() == 1 && swapChain.properties.availableFormats[0].format == VK_FORMAT_UNDEFINED)
				{
					return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
				}

				for (auto i : swapChain.properties.availableFormats)
				{
					if (i.format == VK_FORMAT_R8G8B8A8_UNORM && i.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
					{
						return i;
					}
				}

				return swapChain.properties.availableFormats[0];
			}

			//Select the best available swap mode for the swap chain
			VkPresentModeKHR selectSwapPresentMode(SwapChain& swapChain)
			{
				for (auto i : swapChain.properties.presentModes)
				{
					if (i == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						return i;
					}
				}

				return VK_PRESENT_MODE_FIFO_KHR;
			}

			//Select the extent of the present surface
			VkExtent2D selectSwapExtent(SwapChain& swapChain, WindowInfo& targetWindowInfo)
			{
				if (swapChain.properties.capabilities.currentExtent.width != UINT32_MAX)
				{
					return swapChain.properties.capabilities.currentExtent;
				}
				else
				{
					VkExtent2D actualExtent = { targetWindowInfo.displayWidth, targetWindowInfo.displayHeight };
					actualExtent.width = std::max(swapChain.properties.capabilities.minImageExtent.width, std::min(swapChain.properties.capabilities.maxImageExtent.width, actualExtent.width));
					actualExtent.height = std::max(swapChain.properties.capabilities.minImageExtent.height, std::min(swapChain.properties.capabilities.maxImageExtent.height, actualExtent.height));
					return actualExtent;
				}
			}

			//Create frame buffers for the images in the swap chain
			void createFrameBuffers(Devices& devices, SwapChain& swapChain, Pipeline& pipeline)
			{
				swapChain.frameBuffers.resize(swapChain.imageViews.size());

				for (size_t i = 0; i < swapChain.imageViews.size(); i++)
				{
					std::array<VkImageView, 2> attachments = { swapChain.imageViews[i], swapChain.depthBufferImageView };

					VkFramebufferCreateInfo frameBufferCreateInfo = {};
					frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
					frameBufferCreateInfo.renderPass = swapChain.mainRenderPass;
					frameBufferCreateInfo.pAttachments = attachments.data();
					frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
					frameBufferCreateInfo.width = swapChain.extent.width;
					frameBufferCreateInfo.height = swapChain.extent.height;
					frameBufferCreateInfo.layers = 1;

					if (vkCreateFramebuffer(devices.device, &frameBufferCreateInfo, nullptr, &swapChain.frameBuffers[i]) != VK_SUCCESS)
					{
						throw std::runtime_error("Could not create frame buffer for swap chain image views");
					}
				}
			}

			//Create the swap chain for displaying things to the window
			void createSwapChain(Devices& devices, SwapChain& swapChain, WindowInfo& windowInfo)
			{
				swapChain.properties = findSwapChainProperties(devices.physicalDevice, swapChain);

				VkSurfaceFormatKHR format = selectSwapSurfaceFormat(swapChain);
				VkPresentModeKHR presentMode = selectSwapPresentMode(swapChain);
				VkExtent2D extent = selectSwapExtent(swapChain, windowInfo);

				//Try for triple buffering, or the highest possible image count if it's not available
				uint32_t imageCount = swapChain.properties.capabilities.minImageCount + 1;
				if (swapChain.properties.capabilities.maxImageCount > 0 && swapChain.properties.capabilities.maxImageCount)
				{
					imageCount = std::min((uint32_t)3, swapChain.properties.capabilities.maxImageCount);
				}

				VkSwapchainCreateInfoKHR swapCreateInfo = {};
				swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
				swapCreateInfo.surface = swapChain.renderSurface;
				swapCreateInfo.minImageCount = imageCount;
				swapCreateInfo.imageFormat = format.format;
				swapCreateInfo.imageColorSpace = format.colorSpace;
				swapCreateInfo.imageExtent = extent;
				swapCreateInfo.presentMode = presentMode;
				swapCreateInfo.imageArrayLayers = 1;
				swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; //For final color rendering

				QueueFamilyIndices indices = findQueueFamilies(devices.physicalDevice, swapChain);
				uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily };

				if (indices.graphicsFamily != indices.presentFamily)
				{
					swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
					swapCreateInfo.queueFamilyIndexCount = 2;
					swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
				}
				else
				{
					swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
					swapCreateInfo.queueFamilyIndexCount = 0;
					swapCreateInfo.pQueueFamilyIndices = nullptr;
				}

				swapCreateInfo.preTransform = swapChain.properties.capabilities.currentTransform; //Do not apply any transformations (rotation etc.) to the final image
				swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //Ignore transparency when window blending
				swapCreateInfo.clipped = VK_TRUE; //Allow for the clipping of pixels to improve performance

				swapCreateInfo.oldSwapchain = VK_NULL_HANDLE; //For when the swapchain is recreated. Right now, that is not supported

				if (vkCreateSwapchainKHR(devices.device, &swapCreateInfo, nullptr, &swapChain.swapChain) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create swap chain");
				}

				//Get the handles to the images in the swap chain
				uint32_t numImages = 0;
				vkGetSwapchainImagesKHR(devices.device, swapChain.swapChain, &numImages, nullptr);
				swapChain.images.resize(numImages);
				vkGetSwapchainImagesKHR(devices.device, swapChain.swapChain, &numImages, swapChain.images.data());

				swapChain.extent = extent;
				swapChain.format = format;

				createSwapChainImageViews(devices, swapChain);
			}

			//Recreate the swap chain, used for when the window changes
			void recreateSwapChain(Devices& devices, Buffers& buffers, SwapChain& swapChain, Pipeline& pipeline, WindowInfo& windowInfo)
			{
				vkDeviceWaitIdle(devices.device);
				swapChain.depthFormat = Vulkan::findDepthFormat(devices);

				//clean up the old swap chain
				cleanupSwapchain(devices, swapChain, pipeline);

				//Create a new swap chain
				createSwapChain(devices, swapChain, windowInfo);
				createRenderPass(devices, swapChain, pipeline);

				//Create image views for the images in the swap chain
				createSwapChainImageViews(devices, swapChain);
				//Reinitialize the graphics pipeline
				initializeGraphicsPipeline(devices, pipeline, swapChain);
				createDepthBuffer(devices, buffers, swapChain, pipeline);
				createFrameBuffers(devices, swapChain, pipeline);
			}

			//Get image views for all the images in the swap chain
			void createSwapChainImageViews(Devices& devices, SwapChain& swapChain)
			{
				swapChain.imageViews.resize(swapChain.images.size());

				for (uint32_t i = 0; i < swapChain.images.size(); i++)
				{
					swapChain.imageViews[i] = createImageView(devices, swapChain.images[i], swapChain.format.format);
				}
			}

			//Clean up any resources associated with the swap chain, for swap chain recreation
			void cleanupSwapchain(Devices& devices, SwapChain& swapChain, Pipeline& pipeline)
			{
				vkDestroyImageView(devices.device, swapChain.depthBufferImageView, nullptr);
				vkDestroyImage(devices.device, swapChain.depthBufferImage, nullptr);
				vkFreeMemory(devices.device, swapChain.depthBufferMemory, nullptr);

				for (auto i : swapChain.frameBuffers)
				{
					vkDestroyFramebuffer(devices.device, i, nullptr);
				}

				for (uint32_t i = 0; i < pipeline.commandPools.size(); i++)
				{
					vkFreeCommandBuffers(devices.device, pipeline.commandPools[i], static_cast<uint32_t>(pipeline.commandBuffers[i].size()), pipeline.commandBuffers[i].data());
				}

				vkDestroyPipeline(devices.device, pipeline.renderPipeline, nullptr);
				vkDestroyPipelineLayout(devices.device, pipeline.pipelineLayout, nullptr);
				vkDestroyRenderPass(devices.device, swapChain.mainRenderPass, nullptr);

				for (auto i : swapChain.imageViews)
				{
					vkDestroyImageView(devices.device, i, nullptr);
				}

				vkDestroySwapchainKHR(devices.device, swapChain.swapChain, nullptr);
			}
		}
	}
}
#endif