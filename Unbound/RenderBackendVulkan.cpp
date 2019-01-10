#pragma once
#include "stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include "RenderBackendVulkan.h"

namespace Unbound
{
	namespace Graphics
	{
		void RenderBackendVulkan::init()
		{
			vulkanInit();
		}

		void RenderBackendVulkan::drawFrame()
		{
			//Acquire an image from the swap chain
			uint32_t imageIndex = 0;
			VkResult result = vkAcquireNextImageKHR(devices.device, swapChain.swapChain, UINT64_MAX, semaphores.imageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				Vulkan::recreateSwapChain(devices, buffers, swapChain, pipeline, targetWindowInfo);
				return;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			{
				Logger::log("Error: could not acquire an image on the swap chain for rendering");
				return;
			}

			//Submit the command buffer
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores = { semaphores.imageAcquiredSemaphore };
			VkPipelineStageFlags waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &waitSemaphores;
			submitInfo.pWaitDstStageMask = &waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &pipeline.commandBuffers[0][imageIndex];

			VkSemaphore signalSemaphores = { semaphores.renderFinishedSemaphore };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &signalSemaphores;

			if (vkQueueSubmit(pipeline.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
			{
				Logger::log("Error: could not submit command queue for rendering");
				return;
			}

			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = &signalSemaphores;

			VkSwapchainKHR swapChains = { swapChain.swapChain };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapChains;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pResults = nullptr;

			result = vkQueuePresentKHR(pipeline.presentQueue, &presentInfo);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				Vulkan::recreateSwapChain(devices, buffers, swapChain, pipeline, targetWindowInfo);
			}
			else if (result != VK_SUCCESS)
			{
				Logger::log("Error: could not present swap chain image to window");
				return;
			}

			if (debug.enableValidationLayers)
			{
				vkQueueWaitIdle(pipeline.presentQueue);
			}

			MemoryManager::get()->vertexStagingPool.flush();
			MemoryManager::get()->indexStagingPool.flush();
			MemoryManager::get()->textureStagingPool.flush();
		}

		void RenderBackendVulkan::close()
		{
			vkDeviceWaitIdle(devices.device);

			Vulkan::destroyBuffers(devices, pipeline, buffers);

			Vulkan::destroySemaphores(devices, semaphores);

			Vulkan::cleanupSwapchain(devices, swapChain, pipeline);

			vkDestroyDescriptorPool(devices.device, pipeline.descriptorPool, nullptr);

			//Destroy the descriptor set
			vkDestroyDescriptorSetLayout(devices.device, pipeline.descriptorLayout, nullptr);

			vkDestroyCommandPool(devices.device, pipeline.commandPools[0], nullptr);
			vkDestroyCommandPool(devices.device, pipeline.copyCommandPool, nullptr);

			for (auto i : pipeline.shaderModules)
			{
				vkDestroyShaderModule(devices.device, i, nullptr);
			}

			vkDestroyDevice(devices.device, nullptr);

			if (debug.enableValidationLayers)
			{
				Vulkan::DestroyDebugReportCallbackEXT(vulkanInstance.instance, debug.debugCallbackHandle, nullptr);
			}

			vkDestroySurfaceKHR(vulkanInstance.instance, swapChain.renderSurface, nullptr);
			vkDestroyInstance(vulkanInstance.instance, nullptr);
		}

		//Call to add new shaders to the renderer. Not recommended to call this during runtime as it is quite slow
		void RenderBackendVulkan::addShaders(std::vector<Graphics::Shader>& toAdd)
		{
		}

		//Add new vertex geometry to the renderer
		void RenderBackendVulkan::addGeometry(Mesh& toAdd)
		{
			//Update the mesh's buffer offsets
			toAdd.bufferStartIndex = static_cast<uint32_t>(buffers.numIndices);
			toAdd.vertexOffset = static_cast<uint32_t>(buffers.numVertices);

			//Add the geometry data to the staging buffer
			memcpy(MemoryManager::get()->vertexStagingPool.addData(toAdd.numVertices * sizeof(Vertex)), toAdd.vertices, toAdd.numVertices * sizeof(Vertex));

			memcpy(MemoryManager::get()->indexStagingPool.addData(toAdd.numIndices * sizeof(uint32_t)), toAdd.indices, toAdd.numIndices * sizeof(uint32_t));

			//Copy new vertices into the vertex buffer
			Vulkan::updateVertexBuffer(devices, pipeline, buffers);

			//Update the command buffers with any changes
			updateCommandBuffers();
		}

		//Adds new image data to the render backend, generates required buffers and creates resources
		void RenderBackendVulkan::addImageData(Image& toAdd)
		{
			Vulkan::createTextureImage(devices, swapChain, pipeline, buffers, toAdd, buffers.textureImages[buffers.nextFreeImageIndex], buffers.textureDeviceMemory[buffers.nextFreeImageIndex]);
			buffers.textureImageViews[buffers.nextFreeImageIndex] = Vulkan::createImageView(devices, buffers.textureImages[buffers.nextFreeImageIndex], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);

			//If the next image is free, then increment the next free image index. Otherwise, set it to the end index and increment that
			if (buffers.textureImages[buffers.nextFreeImageIndex + 1] == NULL)
			{
				buffers.nextFreeImageIndex++;
				buffers.endFreeImageIndex++;
			}
			else
			{
				buffers.nextFreeImageIndex = buffers.endFreeImageIndex;
				buffers.endFreeImageIndex++;
			}

			if (pipeline.commandBuffers.size() > 0)
			{
				Vulkan::updateDescriptorSets(devices, pipeline, buffers);
				updateCommandBuffers();
			}
		}

		//Add terrain data to the render backend, using specialized buffers
		void RenderBackendVulkan::addTerrainData(Terrain& toAdd)
		{
			//Add the geometry data to the staging buffers
			for (uint32_t i = 0; i < toAdd.getNumChunks(); i++)
			{
				memcpy(MemoryManager::get()->vertexStagingPool.addData(toAdd.getNumVerticesPerChunk() * sizeof(Vertex)), (toAdd.getData() + i)->vertices, toAdd.getNumVerticesPerChunk() * sizeof(Vertex));
			}

			for (uint32_t i = 0; i < toAdd.getNumChunks(); i++)
			{
				memcpy(MemoryManager::get()->indexStagingPool.addData(toAdd.getNumIndicesPerChunk() * sizeof(uint32_t)), (toAdd.getData() + i)->indices, toAdd.getNumIndicesPerChunk() * sizeof(uint32_t));
			}

			buffers.terrainStartIndex = static_cast<uint32_t>(buffers.numIndices);
			buffers.terrainVertexOffset = static_cast<uint32_t>(buffers.numVertices);
			buffers.numTerrainIndices = toAdd.getNumIndices();
			buffers.numTerrainVertices = toAdd.getNumVertices();

			//Copy new vertices into the vertex buffer
			Vulkan::updateVertexBuffer(devices, pipeline, buffers);

			Logger::log("Loaded terrain into renderer...");

			//Update the command buffers with any changes
			updateCommandBuffers();
		}

		/*Update the structure sent to the vertex shader with new world, view and projection matrices*/
		void RenderBackendVulkan::updateMatrices(const glm::mat4& world, const glm::mat4& view, const glm::mat4& projection)
		{
			buffers.matricesToSend = { view, projection };
			Vulkan::updateUniformBuffer(buffers);
		}

		/*Update only the world matrix. This should usually be done once per object*/
		void RenderBackendVulkan::updateWorldMatrix(const glm::mat4& world)
		{
			buffers.mainPushConstants.world = world;
		}

		/*Update only the view matrix. This only has to be done once per frame*/
		void RenderBackendVulkan::updateViewMatrix(const glm::mat4& view)
		{
			buffers.matricesToSend.view = view;
			Vulkan::updateUniformBuffer(buffers);
		}

		/*Update only the projection matrix. This only has to be done when camera properties change or when the window is resized*/
		void RenderBackendVulkan::updateProjectionMatrix(const glm::mat4& projection)
		{
			buffers.matricesToSend.projection = projection;
			Vulkan::updateUniformBuffer(buffers);
		}

		//Get the next available image index in the renderer, used to load images
		const uint32_t & RenderBackendVulkan::getNextFreeImageIndex() const
		{
			return buffers.nextFreeImageIndex;
		}

		RenderBackendVulkan::RenderBackendVulkan(const WindowInfo& windowInfoToSet, const std::vector<Shader>& shadersToSet) : targetWindowInfo(const_cast<WindowInfo&>(windowInfoToSet)), pipeline(const_cast<std::vector<Graphics::Shader>&>(shadersToSet))
		{
		}

		RenderBackendVulkan::~RenderBackendVulkan()
		{
		}

		/*Calls other functions to fully initialize the vulkan backend.
		 Takes no arguments, but the render window and desired renderer
		 Settings should be loaded before this is called
		*/
		void RenderBackendVulkan::vulkanInit()
		{
			devices.deviceExtensionsToUse = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			Vulkan::createVulkanInstance(devices, vulkanInstance, debug, targetWindowInfo);
			Vulkan::setupDebugCallback(vulkanInstance, debug);
			createVulkanSurface();
			Vulkan::initializePhysicalDevices(devices, swapChain, vulkanInstance);
			Vulkan::createLogicalDevice(devices, swapChain, pipeline);
			Vulkan::createSwapChain(devices, swapChain, targetWindowInfo);
			swapChain.depthFormat = Vulkan::findDepthFormat(devices);
			Vulkan::createTextureSamplers(devices, pipeline);
			Vulkan::createRenderPass(devices, swapChain, pipeline);
			createCommandPool();
			Vulkan::createDescriptorPool(devices, buffers, pipeline);
			Vulkan::createDescriptorSetLayout(devices, buffers, pipeline);
			Vulkan::initializeGraphicsPipeline(devices, pipeline, swapChain);
			Vulkan::createDepthBuffer(devices, buffers, swapChain, pipeline);
			Vulkan::createFrameBuffers(devices, swapChain, pipeline);
			Vulkan::createVertexBuffers(devices, buffers);
			Vulkan::createIndexBuffers(devices, buffers);
			Vulkan::createUniformBuffers(devices, buffers);
			Vulkan::createImageBuffer(devices, pipeline, buffers);
			Vulkan::createDescriptorSets(devices, pipeline, swapChain, buffers);
			createCommandBuffers();
			Vulkan::createSemaphores(devices, semaphores);
			updateCommandBuffers();
			initialized = true;
		}

		/*Creates the render surface that will be presented to*/
		void RenderBackendVulkan::createVulkanSurface()
		{
			if (glfwCreateWindowSurface(vulkanInstance.instance, targetWindowInfo.window, nullptr, &swapChain.renderSurface) != VK_SUCCESS)
			{
				throw std::runtime_error("Could not get target render surface from glfw");
			}
		}

		/*Create the command pool for rendering*/
		void RenderBackendVulkan::createCommandPool()
		{
			//Create the main command pool and command pools for each thread
			Vulkan::QueueFamilyIndices queueIndices = Vulkan::findQueueFamilies(devices.physicalDevice, swapChain);

			VkCommandPoolCreateInfo poolCreateInfo = {};
			poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolCreateInfo.queueFamilyIndex = queueIndices.graphicsFamily;
			poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			for (uint32_t i = 0; i < pipeline.commandPools.size(); i++)
			{
				if (vkCreateCommandPool(devices.device, &poolCreateInfo, nullptr, &pipeline.commandPools[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create command pool");
				}
			}

			//Create the copy command pool
			VkCommandPoolCreateInfo copyPoolCreateInfo = {};
			copyPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			copyPoolCreateInfo.queueFamilyIndex = queueIndices.graphicsFamily;
			copyPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			if (vkCreateCommandPool(devices.device, &copyPoolCreateInfo, nullptr, &pipeline.copyCommandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("Could not create command pool");
			}
		}

		/*Create the command buffers for drawing to the screen*/
		void RenderBackendVulkan::createCommandBuffers()
		{
			//Create command buffers for each image in the swapchain
			pipeline.commandBuffers.resize(swapChain.frameBuffers.size());

			VkCommandBufferAllocateInfo commandBufferInfo = {};
			commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferInfo.commandPool = pipeline.commandPools[0];
			commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferInfo.commandBufferCount = static_cast<uint32_t>(pipeline.commandBuffers[0].size());

			if (vkAllocateCommandBuffers(devices.device, &commandBufferInfo, pipeline.commandBuffers[0].data()) != VK_SUCCESS)
			{
				Logger::log("Error: could not create command buffers for rendering");
				throw std::runtime_error("Could not allocate command buffers for drawing to the screen");
			}

			updateCommandBuffers();
		}

		//Update the command buffers with new instructions
		void RenderBackendVulkan::updateCommandBuffers()
		{
			vkQueueWaitIdle(pipeline.graphicsQueue);
			vkDeviceWaitIdle(devices.device);

			//Record the same intructions in each command buffer
			for (size_t i = 0; i < pipeline.commandBuffers.size(); i++)
			{
				vkResetCommandBuffer(pipeline.commandBuffers[0][i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
				beginInfo.pInheritanceInfo = nullptr;

				if (vkBeginCommandBuffer(pipeline.commandBuffers[0][i], &beginInfo) != VK_SUCCESS)
				{
					Logger::log("Error: could not begin recording commands in command buffer for rendering");
					return;
				}

				VkRenderPassBeginInfo renderBeginInfo = {};
				renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderBeginInfo.renderPass = swapChain.mainRenderPass;
				renderBeginInfo.framebuffer = swapChain.frameBuffers[i];
				renderBeginInfo.renderArea.offset = { 0, 0 };
				renderBeginInfo.renderArea.extent = swapChain.extent;

				std::array<VkClearValue, 2> clearValues = {};
				clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
				clearValues[1].depthStencil = { 1.0f, 0 };

				renderBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
				renderBeginInfo.pClearValues = clearValues.data();

				vkCmdBeginRenderPass(pipeline.commandBuffers[0][i], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

				vkCmdBindPipeline(pipeline.commandBuffers[0][i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.renderPipeline);

				if (initialized)
				{
					//Bind the vertex buffer
					VkBuffer vertexBuffers[] = { buffers.vertexBuffer };
					VkDeviceSize offsets[] = { 0 };
					vkCmdBindVertexBuffers(pipeline.commandBuffers[0][i], 0, 1, vertexBuffers, offsets);

					//Bind the index buffer
					vkCmdBindIndexBuffer(pipeline.commandBuffers[0][i], buffers.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

					//Bind the descriptor sets
					vkCmdBindDescriptorSets(pipeline.commandBuffers[0][i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &pipeline.descriptorSet, 0, nullptr);

					//Start by drawing the terrain
					updateWorldMatrix(glm::mat4());
					buffers.mainPushConstants.albedoID = 0;
					vkCmdPushConstants(pipeline.commandBuffers[0][i], pipeline.pipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof(Vulkan::MainPushConstant), &buffers.mainPushConstants);
					vkCmdDrawIndexed(pipeline.commandBuffers[0][i], buffers.numTerrainIndices, 1, buffers.terrainStartIndex, buffers.terrainVertexOffset, 0);

					for (auto m : *models)
					{
						//Update world matrix
						updateWorldMatrix(m.transform.getTransformMatrix());
						//Push constants
						buffers.mainPushConstants.albedoID = m.material.albedo.getBoundID();
						vkCmdPushConstants(pipeline.commandBuffers[0][i], pipeline.pipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof(Vulkan::MainPushConstant), &buffers.mainPushConstants);
						//Draw the model
						drawModel(m, pipeline.commandBuffers[0][i]);
					}
				}

				vkCmdEndRenderPass(pipeline.commandBuffers[0][i]);

				if (vkEndCommandBuffer(pipeline.commandBuffers[0][i]) != VK_SUCCESS)
				{
					Logger::log("Error: could not write render pass to command buffer");
					return;
				}
			}
		}

		//Draw a model
		void RenderBackendVulkan::drawModel(Model& toDraw, VkCommandBuffer& target)
		{
			//Draw all the model indices
			vkCmdDrawIndexed(target, toDraw.mesh.numIndices, 1, toDraw.mesh.bufferStartIndex, toDraw.mesh.vertexOffset, 0);
		}
	}
}
#endif