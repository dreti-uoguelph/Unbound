#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanRenderPass.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create a render pass to describe buffers through rendering operations
			void createRenderPass(Devices& devices, SwapChain& swapChain, Pipeline& pipeline)
			{
				//Create the color buffer
				VkAttachmentDescription colorAttachment = {};
				colorAttachment.format = swapChain.format.format;
				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //Dependent on multisampling
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //Not currently using stencil buffer
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //Not currently using stencil buffer
				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //Since the image is used in the swap chain

				VkAttachmentReference colorAttachmentReference = {};
				colorAttachmentReference.attachment = 0; //Index in the array of attachments to refer to
				colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

				VkSubpassDependency colorSubpassDependency = {};
				colorSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				colorSubpassDependency.dstSubpass = 0;
				colorSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				colorSubpassDependency.srcAccessMask = 0;
				colorSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				colorSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				//Create the depth attachment and pass
				VkAttachmentDescription depthAttachmentDescription = {};
				depthAttachmentDescription.format = swapChain.depthFormat;
				depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkAttachmentReference depthAttachmentReference = {};
				depthAttachmentReference.attachment = 1;
				depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				VkSubpassDescription subpass = {};
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments = &colorAttachmentReference;
				subpass.pDepthStencilAttachment = &depthAttachmentReference;

				std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachmentDescription };
				VkRenderPassCreateInfo renderPassCreateInfo = {};
				renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
				renderPassCreateInfo.pAttachments = attachments.data();
				renderPassCreateInfo.subpassCount = 1;
				renderPassCreateInfo.pSubpasses = &subpass;
				renderPassCreateInfo.dependencyCount = 1;
				renderPassCreateInfo.pDependencies = &colorSubpassDependency;

				if (vkCreateRenderPass(devices.device, &renderPassCreateInfo, nullptr, &swapChain.mainRenderPass))
				{
					throw std::runtime_error("Could not create main render pass");
				}
			}
		}
	}
}
#endif