#pragma once
#ifdef UNBOUND_BACKEND_VULKAN
#include"stdafx.h"
#include"RenderBackend.h"
#include"glm\gtc\matrix_transform.hpp"
#include"Mesh.h"
#include"Terrain.h"
#include"ImageLoader.h"
#include"MemoryManager.h"
#include"Model.h"
#include"VulkanPipeline.h"
#include"VulkanSwapchain.h"
#include"VulkanSemaphore.h"
#include"VulkanDescriptorSets.h"
#include"VulkanDebug.h"
#include"VulkanInstance.h"

namespace Unbound
{
	namespace Graphics
	{
		class RenderBackendVulkan : public RenderBackend
		{
		public:
			//Called when the render backend is initialized
			virtual void init() override;
			//Called when the render backend draws a frame
			virtual void drawFrame() override;
			//Called when the render backend should be closed
			virtual void close() override;

			//Call to add new shaders to the renderer. Not recommended to call this during runtime as it is quite slow
			virtual void addShaders(std::vector<Graphics::Shader>& toAdd) override;
			//Add new vertex geometry to the renderer
			virtual void addGeometry(Mesh& toAdd) override;
			//Uploads new image data to the renderer
			virtual void addImageData(Image& toAdd) override;
			//Add terrain data to the render backend, using specialized buffers
			virtual void addTerrainData(Terrain& toAdd) override;

			//Update the structure sent to the vertex shader with new world, view and projection matrices
			void updateMatrices(const glm::mat4& world, const glm::mat4& view, const glm::mat4& projection);
			//Update only the world matrix. This should usually be done once per object
			void updateWorldMatrix(const glm::mat4& world);
			//Update only the view matrix. This only has to be done once per frame
			void updateViewMatrix(const glm::mat4& view);
			//Update only the projection matrix. This only has to be done when camera properties change or when the window is resized
			void updateProjectionMatrix(const glm::mat4& projection);

			//Get the next available image index, which is used for loading images into the renderer
			const uint32_t& getNextFreeImageIndex() const;

			std::vector<Model>* models; //All the models that are currently in the scene

			RenderBackendVulkan(const WindowInfo& windowInfoToSet, const std::vector<Shader>& shadersToSet);
			~RenderBackendVulkan();
		private:

			WindowInfo & targetWindowInfo; //Information about the target window
			Vulkan::VulkanInstance vulkanInstance; //Current vulkan instance
			Vulkan::Devices devices; //The devices associated with this vulkan context
			Vulkan::SwapChain swapChain; //The struct containing swap chain variables
			Vulkan::Pipeline pipeline; //The render pipeline
			Vulkan::Buffers buffers; //The different buffers in use
			Vulkan::Semaphores semaphores; //Semaphores for rendering
			Vulkan::VulkanDebug debug; //Struct with debug info

			//Calls other functions to fully initialize the vulkan backend
			void vulkanInit();
			//Creates the render surface that will be presented to
			void createVulkanSurface();
			//Create the command pool for rendering
			void createCommandPool();
			//Create the command buffers for drawing to the screen
			void createCommandBuffers();

			//Update the command buffers with new instructions
			void updateCommandBuffers();

			//Draw a model
			void drawModel(Model& toDraw, VkCommandBuffer& target);
		};
	}
}
#endif