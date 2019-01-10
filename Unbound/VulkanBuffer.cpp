#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanBuffer.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Creates a command buffer for storing commands that only needed to be executed once
			void beginSingleSubmitCommands(Devices& devices, Buffers& buffers, VkCommandPool& cmdPool)
			{
				//If the single submit command buffer is not currently allocated, do that first
				if (!buffers.singleSubmitBufferReady)
				{
					VkCommandBufferAllocateInfo allocInfo = {};
					allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
					allocInfo.commandPool = cmdPool;
					allocInfo.commandBufferCount = 1;
					allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

					vkAllocateCommandBuffers(devices.device, &allocInfo, &buffers.singleSubmitBuffer);
					buffers.singleSubmitBufferReady = true;
				}
				else
				{
					//Otherwise, reset the command buffer
					vkResetCommandBuffer(buffers.singleSubmitBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
				}

				//Begin adding commands to the buffer
				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(buffers.singleSubmitBuffer, &beginInfo);
			}

			//Ends the recording of commands for single submission
			void endSingleSubmitCommands(Devices& devices, Pipeline& pipeline, VkCommandBuffer commandBuffer, VkCommandPool poolToFreeFrom)
			{
				vkEndCommandBuffer(commandBuffer);

				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;

				vkQueueSubmit(pipeline.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(pipeline.graphicsQueue);
			}

			//Copy size bytes from one buffer into another buffer starting at offset
			void copyBuffer(Devices& devices, Buffers& buffers, Pipeline& pipeline, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize offset)
			{
				//The region to copy from and to. Change this later. For now, it's a one-to-one mapping
				VkBufferCopy copyRegion = {};
				copyRegion.srcOffset = offset;
				copyRegion.dstOffset = offset;
				copyRegion.size = size;

				Vulkan::beginSingleSubmitCommands(devices, buffers, pipeline.copyCommandPool);
				vkCmdCopyBuffer(buffers.singleSubmitBuffer, srcBuffer, dstBuffer, 1, &copyRegion); //In the future, we could even copy to multiple regions at the same time!
				Vulkan::endSingleSubmitCommands(devices, pipeline, buffers.singleSubmitBuffer, pipeline.copyCommandPool);
			}

			//Copies the contents of a buffer into an already initialized image
			void copyBufferToImage(Devices& devices, Buffers& buffers, Pipeline& pipeline, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
			{
				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferImageHeight = 0;
				copyRegion.bufferRowLength = 0;
				copyRegion.bufferOffset = 0;

				copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount = 1;
				copyRegion.imageSubresource.mipLevel = 0;

				copyRegion.imageOffset = { 0, 0, 0 };
				copyRegion.imageExtent = { width, height, 1 };

				Vulkan::beginSingleSubmitCommands(devices, buffers, pipeline.copyCommandPool);
				vkCmdCopyBufferToImage(buffers.singleSubmitBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
				Vulkan::endSingleSubmitCommands(devices, pipeline, buffers.singleSubmitBuffer, pipeline.copyCommandPool);
			}

			//Destroys any buffers that were created for rendering
			void destroyBuffers(Devices& devices, Pipeline& pipeline, Buffers& buffers)
			{
				vkUnmapMemory(devices.device, buffers.textureStagingBufferMemory);
				vkUnmapMemory(devices.device, buffers.vertexStagingBufferMemory);
				vkUnmapMemory(devices.device, buffers.indexStagingBufferMemory);
				vkUnmapMemory(devices.device, buffers.uniformBufferMemory);

				//Destroy the uniform buffer
				vkDestroyBuffer(devices.device, buffers.uniformBuffer, nullptr);
				vkFreeMemory(devices.device, buffers.uniformBufferMemory, nullptr);

				//Destroy vertex buffer
				vkDestroyBuffer(devices.device, buffers.vertexStagingBuffer, nullptr);
				vkFreeMemory(devices.device, buffers.vertexStagingBufferMemory, nullptr);
				vkDestroyBuffer(devices.device, buffers.vertexBuffer, nullptr);
				vkFreeMemory(devices.device, buffers.vertexBufferMemory, nullptr);

				//Destroy index buffer
				vkDestroyBuffer(devices.device, buffers.indexStagingBuffer, nullptr);
				vkFreeMemory(devices.device, buffers.indexStagingBufferMemory, nullptr);
				vkDestroyBuffer(devices.device, buffers.indexBuffer, nullptr);
				vkFreeMemory(devices.device, buffers.indexBufferMemory, nullptr);

				//Destroy image buffers
				vkDestroySampler(devices.device, pipeline.textureSampler, nullptr);

				for (uint32_t i = 0; i < buffers.numberOfImages; i++)
				{
					vkDestroyImageView(devices.device, buffers.textureImageViews[i], nullptr);
				}

				vkDestroyBuffer(devices.device, buffers.textureStagingBuffer, nullptr);
				vkFreeMemory(devices.device, buffers.textureStagingBufferMemory, nullptr);
			}

			//Create a new buffer using the specified memory settings
			void createBuffer(Devices& devices, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
			{
				VkBufferCreateInfo bufferInfo = {};
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = size;
				bufferInfo.usage = usage;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

				if (vkCreateBuffer(devices.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create buffer for rendering");
				}

				VkMemoryRequirements bufferMemoryRequirements = {};
				vkGetBufferMemoryRequirements(devices.device, buffer, &bufferMemoryRequirements);

				VkMemoryAllocateInfo allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = bufferMemoryRequirements.size;
				allocInfo.memoryTypeIndex = Vulkan::findMemoryType(devices, bufferMemoryRequirements.memoryTypeBits, properties);

				if (vkAllocateMemory(devices.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not allocate memory to buffer when rendering");
				}

				vkBindBufferMemory(devices.device, buffer, bufferMemory, 0);
			}

			//Create the vertex buffers to send vertices to the GPU
			void createVertexBuffers(Devices& devices, Buffers& buffers)
			{
				buffers.vertexBufferSize = MemoryManager::get()->vertexDataPool.getSize();
				buffers.vertexStagingBufferSize = MemoryManager::get()->vertexStagingPool.getSize();

				//Create and map the staging buffer
				createBuffer(devices, buffers.vertexStagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffers.vertexStagingBuffer, buffers.vertexStagingBufferMemory);

				void* data;
				vkMapMemory(devices.device, buffers.vertexStagingBufferMemory, 0, buffers.vertexStagingBufferSize, 0, &data);
				MemoryManager::get()->vertexStagingPool.init(data);

				//Create the vertex buffer
				createBuffer(devices, buffers.vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffers.vertexBuffer, buffers.vertexBufferMemory);
			}

			//Create the staging buffers for sending images to the GPU
			void createImageBuffer(Devices& devices, Pipeline& pipeline, Buffers& buffers)
			{
				void* data;
				buffers.textureStagingBufferSize = MemoryManager::get()->textureStagingPool.getSize();

				createBuffer(devices, buffers.textureStagingBufferSize, VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffers.textureStagingBuffer, buffers.textureStagingBufferMemory);

				//Map the memory in the staging memory pool
				vkMapMemory(devices.device, buffers.textureStagingBufferMemory, 0, buffers.textureStagingBufferSize, 0, reinterpret_cast<void**>(&data));
				MemoryManager::get()->textureStagingPool.init(data);
			}

			//Create the index buffers to send indices to the GPU
			void createIndexBuffers(Devices& devices, Buffers& buffers)
			{
				buffers.indexBufferSize = MemoryManager::get()->indexDataPool.getSize();
				buffers.indexStagingBufferSize = MemoryManager::get()->indexStagingPool.getSize();

				//Create the staging buffer, which is smaller than the full index buffer
				createBuffer(devices, buffers.indexStagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffers.indexStagingBuffer, buffers.indexStagingBufferMemory);

				void* data;
				vkMapMemory(devices.device, buffers.indexStagingBufferMemory, 0, buffers.indexStagingBufferSize, 0, &data);
				MemoryManager::get()->indexStagingPool.init(data);

				//Create the index buffer
				createBuffer(devices, buffers.indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffers.indexBuffer, buffers.indexBufferMemory);
			}

			//Create the uniform buffers for sending uniforms to the GPU
			void createUniformBuffers(Devices& devices, Buffers& buffers)
			{
				VkDeviceSize bufferSize = sizeof(MatrixLayout);
				createBuffer(devices, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffers.uniformBuffer, buffers.uniformBufferMemory);
				vkMapMemory(devices.device, buffers.uniformBufferMemory, 0, sizeof(MatrixLayout), 0, &buffers.uniformBufferData);
			}

			//Update the uniform buffer with new values
			void updateUniformBuffer(Buffers& buffers)
			{
				memcpy(buffers.uniformBufferData, &buffers.matricesToSend, sizeof(MatrixLayout));
			}

			//Copy newly added values in the vertex staging buffer to the vertex buffer
			void updateVertexBuffer(Devices& devices, Pipeline& pipeline, Buffers& buffers)
			{
				//Copy the contents of the staging buffer into the vertex buffer
				uint32_t numNewVertices = static_cast<uint32_t>(MemoryManager::get()->vertexStagingPool.getCurrentUsage() / sizeof(Vertex)) - static_cast<uint32_t>(buffers.numVertices);
				copyBuffer(devices, buffers, pipeline, buffers.vertexStagingBuffer, buffers.vertexBuffer, sizeof(Vertex) * numNewVertices, buffers.numVertices * sizeof(Vertex));

				uint32_t numNewIndices = static_cast<uint32_t>(MemoryManager::get()->indexStagingPool.getCurrentUsage() / sizeof(uint32_t)) - static_cast<uint32_t>(buffers.numIndices);
				copyBuffer(devices, buffers, pipeline, buffers.indexStagingBuffer, buffers.indexBuffer, sizeof(uint32_t) * numNewIndices, buffers.numIndices * sizeof(uint32_t));

				buffers.numVertices += numNewVertices;
				buffers.numIndices += numNewIndices;
			}
		}
	}
}
#endif