#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanDescriptorSets.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Updates the descriptor sets to handle new image bindings
			void updateDescriptorSets(Devices& devices, Pipeline& pipeline, Buffers& buffers)
			{
				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = buffers.uniformBuffer;
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(Vulkan::MatrixLayout);

				VkDescriptorImageInfo* imageInfos = new VkDescriptorImageInfo[buffers.numberOfImages];

				for (uint32_t i = 0; i < buffers.numberOfImages; i++)
				{
					imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfos[i].imageView = buffers.textureImageViews[i];
					imageInfos[i].sampler = pipeline.textureSampler;
				}

				pipeline.descriptorWrites[0].pBufferInfo = &bufferInfo;
				pipeline.descriptorWrites[2].descriptorCount = buffers.numberOfImages;
				pipeline.descriptorWrites[2].pImageInfo = imageInfos;

				vkUpdateDescriptorSets(devices.device, static_cast<uint32_t>(pipeline.descriptorWrites.size()), pipeline.descriptorWrites.data(), 0, nullptr);
			}

			//Create the descriptor pool for allocating desciptor buffers
			void createDescriptorPool(Devices& devices, Buffers& buffers, Pipeline& pipeline)
			{
				std::array<VkDescriptorPoolSize, 3> poolSizes;
				poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				poolSizes[0].descriptorCount = 1;
				poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
				poolSizes[1].descriptorCount = 1;
				poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				poolSizes[2].descriptorCount = buffers.numberOfImages;

				VkDescriptorPoolCreateInfo poolCreateInfo = {};
				poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
				poolCreateInfo.pPoolSizes = poolSizes.data();
				poolCreateInfo.maxSets = 1; //Only one descriptor set will be allocated
				poolCreateInfo.flags = 0;

				if (vkCreateDescriptorPool(devices.device, &poolCreateInfo, nullptr, &pipeline.descriptorPool) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create descriptor pool for allocating descriptor sets");
				}
			}

			//Create the descriptor sets required for sending uniforms to shaders
			void createDescriptorSets(Devices& devices, Pipeline& pipeline, SwapChain& swapChain, Buffers& buffers)
			{
				VkDescriptorSetLayout layouts[] = { pipeline.descriptorLayout };

				VkDescriptorSetAllocateInfo allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				allocInfo.descriptorPool = pipeline.descriptorPool;
				allocInfo.descriptorSetCount = 1;
				allocInfo.pSetLayouts = &pipeline.descriptorLayout;

				if (vkAllocateDescriptorSets(devices.device, &allocInfo, &pipeline.descriptorSet) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not allocate descriptor sets for sending uniforms to shaders");
				}

				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = buffers.uniformBuffer;
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(Vulkan::MatrixLayout);

				//Build an image info struct for each image
				VkDescriptorImageInfo* imageInfos = new VkDescriptorImageInfo[buffers.numberOfImages];
				buffers.textureImages = new VkImage[buffers.numberOfImages];
				buffers.textureImageViews = new VkImageView[buffers.numberOfImages];
				buffers.textureDeviceMemory = new VkDeviceMemory[buffers.numberOfImages];

				//Load the blank image
				Image blankImg = IO::ImageLoader::loadFromFile("Assets/Textures/blank.png");

				//Add the blank image to the graphics pipeline
				createTextureImage(devices, swapChain, pipeline, buffers, blankImg, buffers.textureImages[0], buffers.textureDeviceMemory[0]);
				buffers.textureImageViews[0] = createImageView(devices, buffers.textureImages[0], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
				imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				buffers.nextFreeImageIndex = 1;
				buffers.endFreeImageIndex = 1;

				//Create a view for each image
				for (uint32_t i = 0; i < buffers.numberOfImages; i++)
				{
					buffers.textureImageViews[i] = buffers.textureImageViews[0];
					imageInfos[i].sampler = pipeline.textureSampler;
					imageInfos[i].imageView = buffers.textureImageViews[i];
					imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				}

				pipeline.descriptorWrites[0] = {};
				pipeline.descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				pipeline.descriptorWrites[0].dstSet = pipeline.descriptorSet;
				pipeline.descriptorWrites[0].dstBinding = 0;
				pipeline.descriptorWrites[0].dstArrayElement = 0;
				pipeline.descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				pipeline.descriptorWrites[0].descriptorCount = 1;
				pipeline.descriptorWrites[0].pBufferInfo = &bufferInfo;
				pipeline.descriptorWrites[0].pImageInfo = nullptr;
				pipeline.descriptorWrites[0].pTexelBufferView = nullptr;

				pipeline.descriptorWrites[1] = {};
				pipeline.descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				pipeline.descriptorWrites[1].dstSet = pipeline.descriptorSet;
				pipeline.descriptorWrites[1].dstBinding = 1;
				pipeline.descriptorWrites[1].dstArrayElement = 0;
				pipeline.descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				pipeline.descriptorWrites[1].descriptorCount = 1;
				pipeline.descriptorWrites[1].pBufferInfo = nullptr;
				pipeline.descriptorWrites[1].pImageInfo = imageInfos;
				pipeline.descriptorWrites[1].pTexelBufferView = nullptr;

				pipeline.descriptorWrites[2] = {};
				pipeline.descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				pipeline.descriptorWrites[2].dstSet = pipeline.descriptorSet;
				pipeline.descriptorWrites[2].dstBinding = 2;
				pipeline.descriptorWrites[2].dstArrayElement = 0;
				pipeline.descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				pipeline.descriptorWrites[2].descriptorCount = buffers.numberOfImages;
				pipeline.descriptorWrites[2].pBufferInfo = nullptr;
				pipeline.descriptorWrites[2].pImageInfo = imageInfos;
				pipeline.descriptorWrites[2].pTexelBufferView = nullptr;

				vkUpdateDescriptorSets(devices.device, static_cast<uint32_t>(pipeline.descriptorWrites.size()), pipeline.descriptorWrites.data(), 0, nullptr);
			}

			//Create the descriptor set layout for setting uniforms in vulkan
			void createDescriptorSetLayout(Devices& devices, Buffers& buffers, Pipeline& pipeline)
			{
				VkDescriptorSetLayoutBinding matrixBinding = {};
				matrixBinding.binding = 0;
				matrixBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				matrixBinding.descriptorCount = 1;
				matrixBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				matrixBinding.pImmutableSamplers = nullptr;

				VkDescriptorSetLayoutBinding samplerBinding = {};
				samplerBinding.binding = 1;
				samplerBinding.descriptorCount = 1;
				samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
				samplerBinding.pImmutableSamplers = &pipeline.textureSampler;
				samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

				VkDescriptorSetLayoutBinding imagesBinding = {};
				imagesBinding.binding = 2;
				imagesBinding.descriptorCount = buffers.numberOfImages;
				imagesBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				imagesBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				imagesBinding.pImmutableSamplers = nullptr;

				std::array<VkDescriptorSetLayoutBinding, 3> setBindings = { matrixBinding, samplerBinding, imagesBinding };
				VkDescriptorSetLayoutCreateInfo createInfo = {};
				createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				createInfo.bindingCount = static_cast<uint32_t>(setBindings.size());
				createInfo.pBindings = setBindings.data();

				if (vkCreateDescriptorSetLayout(devices.device, &createInfo, nullptr, &pipeline.descriptorLayout) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create descriptor set for sending uniforms to shaders");
				}
			}
		}
	}
}
#endif