#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"Shader.h"
#include"Logger.h"
#include<array>
#include<map>
#ifdef _WIN32
#undef min
#undef max
#endif

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			/*A struct to represent the matrix layout sent to the vertex shader*/
			struct MatrixLayout
			{
				glm::mat4 view;
				glm::mat4 projection;
			};

			/*Struct representing a push constant to send to shaders*/
			struct MainPushConstant
			{
				uint32_t albedoID;
				uint32_t normalID;
				uint32_t roughnessMetalnessID;
				uint32_t padding1;
				glm::mat4 world;
			};

			/*Stores the vulkan instance and related data*/
			struct VulkanInstance
			{
				VkInstance instance;
			};

			/*Stores data relating to debugging validation layers and other debugging tools*/
			struct VulkanDebug
			{
#ifdef _DEBUG
				const bool enableValidationLayers = true;
#else
				const bool enableValidationLayers = false;
#endif
				VkDebugReportCallbackEXT debugCallbackHandle; //Handle for the debug callback function
				std::vector<const char*> validationLayersToUse; //The names of the validation layers to use (for debugging vulkan)
			};

			/*Stores all the buffers that are used by the renderer*/
			struct Buffers
			{
				VkBuffer vertexBuffer; //Vertex buffer for drawing vertices to the screen
				VkDeviceMemory vertexBufferMemory; //Memory region representing the vertices in the scene
				VkBuffer vertexStagingBuffer; //The staging buffer for vertices
				VkDeviceMemory vertexStagingBufferMemory; //The memory region representing the vertex staging buffer
				uint64_t vertexBufferSize = 0; //The size of the vertex buffer on the GPU, in bytes
				uint64_t vertexStagingBufferSize = 0; //The size of the vertex staging buffer in bytes

				VkBuffer indexBuffer; //Index buffer for drawing indices to the screen
				VkDeviceMemory indexBufferMemory; //Memory region representing the indices in the scene
				VkBuffer indexStagingBuffer; //The staging buffer for indices
				VkDeviceMemory indexStagingBufferMemory;  //The region of memory representing the index staging buffer
				uint64_t indexBufferSize = 0; //Size of the index buffer in bytes
				uint64_t indexStagingBufferSize = 0; //Size of the index staging buffer in bytes

				VkBuffer uniformBuffer; //The buffer containing uniforms to send to shaders
				VkDeviceMemory uniformBufferMemory; //Memory region representing uniforms to send to shaders
				void* uniformBufferData; //The data belonging to the uniform buffer

				VkBuffer textureStagingBuffer; //The buffer for storing image data on the GPU
				VkDeviceMemory textureStagingBufferMemory; //Represents the memory held bu the texture buffer
				uint64_t textureStagingBufferSize; //Size of the texture staging buffer, in bytes
				uint32_t numberOfImages = 4096; //The maximum number of images in the frame. This can be changed, but then the descriptor sets have to be regenerated
				VkImage* textureImages; //The texture images in the renderer, this is numberOfImages in size
				VkImageView* textureImageViews; //These are the image views into the texture images, of the same size
				VkDeviceMemory* textureDeviceMemory; //The memory assigned to the texture buffer
				uint32_t endFreeImageIndex = 0; //The index of the free image at the end of a chunk of allocated images.
				uint32_t nextFreeImageIndex = 0; //The index of the next image available for writing
				uint64_t vertexBufferDynamicOffset = 0; //The offset to the dynamic section of the vertex buffer. Vertices before this are only loaded at the start of the level
				uint64_t indexBufferDynamicOffset = 0; //The offset to the dynamic section of the index buffer
				uint64_t textureBufferDynamicOffset = 0; //The offset to the dynamic section of the texture buffer
				uint64_t numVertices = 0;
				uint64_t numIndices = 0;
				uint64_t numFaces = 0;
				uint32_t numTerrainVertices = 0;
				uint32_t numTerrainIndices = 0;
				uint32_t terrainStartIndex = 0;
				uint32_t terrainVertexOffset = 0;

				Vulkan::MatrixLayout matricesToSend; //The current matrix sent to the vertex shader
				Vulkan::MainPushConstant mainPushConstants; //Push constants for fragment shader

				VkCommandBuffer singleSubmitBuffer;
				bool singleSubmitBufferReady;

				Buffers()
				{
					singleSubmitBufferReady = false;
				}

				~Buffers()
				{
					delete[] textureImages;
					delete[] textureImageViews;
					delete[] textureDeviceMemory;
				}
			};

			/*A struct to store info about vulkan queue families*/
			struct QueueFamilyIndices
			{
				int graphicsFamily = -1;
				int presentFamily = -1;

				bool isComplete()
				{
					return graphicsFamily >= 0 && presentFamily >= 0;
				}
			};

			/*A struct to store information about swap chain properties*/
			struct SwapChainProperties
			{
				VkSurfaceCapabilitiesKHR capabilities;
				std::vector<VkSurfaceFormatKHR> availableFormats;
				std::vector<VkPresentModeKHR> presentModes;
			};

			struct Semaphores
			{
				VkSemaphore imageAcquiredSemaphore; //Semaphore to signal that an image has been acquired for rendering
				VkSemaphore renderFinishedSemaphore; //Semaphore to signal that an image finished rendering and is ready to be presented
			};

			/*Struct containing variables related to the swapchain*/
			struct SwapChain
			{
				VkSwapchainKHR swapChain; //A swap chain for rendering to the window
				VkSurfaceFormatKHR format; //The format of the images in the swap chain
				VkExtent2D extent; //The extent, or size of the images in the swap chain
				VkRenderPass mainRenderPass; //The main rendering pass that will be used in the engine
				VkSurfaceKHR renderSurface; //Representation of the target render surface
				SwapChainProperties properties; //The properties of this swap chain
				VkImage depthBufferImage; //The image representing the depth buffer
				VkImageView depthBufferImageView; //The image view for the depth image
				VkDeviceMemory depthBufferMemory; //Represents the memory region associated witht the depth buffer
				VkFormat depthFormat; //The format of the depth buffer
				std::vector<VkImage> images; //Handles to images that make up the swap chain
				std::vector<VkImageView> imageViews; //Image views for the images in the swap chain, one for each
				std::vector<VkFramebuffer> frameBuffers; //Frame buffers representing the images in the swap chain
			};

			/*Struct representing all the devices and data associated with devices in vulkan*/
			struct Devices
			{
				VkDevice device; //The current vulkan logical device
				std::vector<VkExtensionProperties> vulkanExtensions; //A set of supported vulkan extensions on this system
				std::vector<VkPhysicalDevice> physicalDevices; //A set of all the physical devices currently in the system
				std::multimap<int, VkPhysicalDevice> deviceRatings; //The rating of each device in the system. More powerful devices have higher ratings so that they are used
				std::vector<const char*> deviceExtensionsToUse; //The names of device extensions to use
				VkPhysicalDevice physicalDevice; //Current vulkan physical device used for rendering

				Devices()
				{
					vulkanExtensions.reserve(24);
					physicalDevices.reserve(4);
					deviceExtensionsToUse.reserve(1024);
				}
			};

			/*Struct containing the variables most associated with the pipeline*/
			struct Pipeline
			{
				VkQueue graphicsQueue; //A handle to the graphics queue family on the vulkan device
				VkQueue presentQueue; //A handle to the present queue on the vulkan device
				std::vector<Shader>& generalShaders; //The shaders containing the bytecode that will be loaded into the shader modules
				std::vector<VkShaderModule> shaderModules; //Stores the vulkan shader modules corrosponding to all the shaders in the scene
				VkDescriptorPool descriptorPool; //A descriptor pool for allocating descriptor sets for shader uniforms
				VkDescriptorSetLayout descriptorLayout; //The descriptor set layout for the uniform buffers used for rendering
				VkDescriptorSet descriptorSet; //Descriptor set for sending uniforms to the shader for rendering
				VkPipelineLayout pipelineLayout; //The layout of the uniforms passed to shaders
				VkPipeline renderPipeline; //Handle to the pipeline that will be used for rendering

				std::vector<VkCommandPool> commandPools; //The command pool that will be used for rendering
				std::vector<std::vector<VkCommandBuffer>> commandBuffers; //All the command buffers currently in use

				VkCommandPool copyCommandPool; //Command pool used for small and frequent copying operations
				VkSampler textureSampler;
				std::array<VkWriteDescriptorSet, 3> descriptorWrites;

				std::vector<VkFence> commandBufferFreeFences;

				Pipeline(std::vector<Shader>& shaders) : generalShaders(shaders)
				{
					commandPools.reserve(4);
					commandBuffers.reserve(4);
					commandBufferFreeFences.reserve(12);

					for (auto i : commandBuffers)
					{
						i.reserve(3);
					}

					for (int i = 0; i < 4; i++)
					{
						commandPools.push_back(VkCommandPool());
						commandBuffers.push_back(std::vector<VkCommandBuffer>());

						for (int j = 0; j < 3; j++)
						{
							commandBuffers[i].push_back(VkCommandBuffer());
						}
					}
				}
			};
		}
	}
}
#endif