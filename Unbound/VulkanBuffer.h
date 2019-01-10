#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanStructs.h"
#include"VulkanMemory.h"
#include"MemoryManager.h"
#include"Vertex.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Creates a command buffer for storing commands that only needed to be executed once
			void beginSingleSubmitCommands(Devices& devices, Buffers& buffers, VkCommandPool& cmdpool);
			//Ends the recording of commands for single submission
			void endSingleSubmitCommands(Devices& devices, Pipeline& pipeline, VkCommandBuffer commandBuffer, VkCommandPool poolToFreeFrom);
			//Copy size bytes from one buffer into another buffer starting at offset
			void copyBuffer(Devices& devices, Buffers& buffers, Pipeline& pipeline, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize offset);
			//Copies the contents of a buffer into an already initialized image
			void copyBufferToImage(Devices& devices, Buffers& buffers, Pipeline& pipeline, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
			//Create a new buffer using the specified memory settings
			void createBuffer(Devices& devices, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			//Create the vertex buffers to send vertices to the GPU
			void createVertexBuffers(Devices& devices, Buffers& buffers);
			//Create the staging buffers for sending images to the GPU
			void createImageBuffer(Devices& devices, Pipeline& pipeline, Buffers& buffers);
			//Create the index buffers to send indices to the GPU
			void createIndexBuffers(Devices& devices, Buffers& buffers);
			//Create the uniform buffers for sending uniforms to the GPU
			void createUniformBuffers(Devices& device, Buffers& buffers);

			//Update the uniform buffer with new values
			void updateUniformBuffer(Buffers& buffers);
			//Copy newly added values in the vertex staging buffer to the vertex buffer
			void updateVertexBuffer(Devices& devices, Pipeline& pipeline, Buffers& buffers);

			//Destroys any buffers that were created for rendering
			void destroyBuffers(Devices& devices, Pipeline& pipeline, Buffers& buffers);
		}
	}
}
#endif