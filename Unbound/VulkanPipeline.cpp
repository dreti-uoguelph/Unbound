#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanPipeline.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Initialize the graphics pipeline, preparing it for rendering
			void initializeGraphicsPipeline(Devices& devices, Pipeline& pipeline, SwapChain& swapChain)
			{
				//A cache of the pipeline, to be loaded and written
				VkPipelineCache pipelineCache;
				bool writeCache = true;

				std::fstream pipelineCacheFile;
				pipelineCacheFile.open("pipelinecache", std::ios::in | std::ios::binary | std::ios::ate);
				uint32_t fileSize = static_cast<uint32_t>(pipelineCacheFile.tellg());
				pipelineCacheFile.seekg(0);

				//If the pipeline cache already exists, load it in and apply it
				if (pipelineCacheFile.is_open() && fileSize > 0)
				{
					void* fileData = malloc(fileSize);
					pipelineCacheFile.read(reinterpret_cast<char*>(fileData), fileSize);

					VkPipelineCacheCreateInfo cacheCreateInfo = {};
					cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
					cacheCreateInfo.initialDataSize = fileSize;
					cacheCreateInfo.pInitialData = fileData;

					if (vkCreatePipelineCache(devices.device, &cacheCreateInfo, nullptr, &pipelineCache) != VK_SUCCESS)
					{
						throw std::runtime_error("Could not load pipeline cache from file. Delete the file and run again");
					}

					Logger::log("Renderer: successfully loaded pipeline cache");
					writeCache = false;
				}
				else
				{
					VkPipelineCacheCreateInfo cacheCreateInfo = {};
					cacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
					cacheCreateInfo.initialDataSize = 0;
					cacheCreateInfo.pInitialData = VK_NULL_HANDLE;

					if (vkCreatePipelineCache(devices.device, &cacheCreateInfo, nullptr, &pipelineCache) != VK_SUCCESS)
					{
						throw std::runtime_error("Could not create pipeline cache to store data in");
					}

					Logger::log("Warning: renderer could not find pipeline cache, rebuilding...");
				}

				pipelineCacheFile.close();

				//Load the general shaders into shader modules. The first two shaders are always the main vertex and pixel shader
				pipeline.shaderModules.resize(pipeline.generalShaders.size());

				int j = 0;
				for (auto i : pipeline.generalShaders)
				{
					pipeline.shaderModules[j] = createShaderModule(devices.device, const_cast<std::vector<char>&>(i.getBuffer()));
					j++;
				}

				//Create the main vertex shader
				VkPipelineShaderStageCreateInfo vertexShaderInfo = {};
				vertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertexShaderInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertexShaderInfo.pName = "main";
				vertexShaderInfo.module = pipeline.shaderModules[0];
				vertexShaderInfo.pSpecializationInfo = nullptr; //Info for setting constants

				//Create the main fragment shader
				VkPipelineShaderStageCreateInfo fragmentShaderInfo = {};
				fragmentShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragmentShaderInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragmentShaderInfo.pName = "main";
				fragmentShaderInfo.module = pipeline.shaderModules[1];
				fragmentShaderInfo.pSpecializationInfo = nullptr; //Info for setting constants

				//Put the shader creation info into an array
				VkPipelineShaderStageCreateInfo shaderStageCreationInfo[] = { vertexShaderInfo, fragmentShaderInfo };

				//Get the vertex input information for constructing the pipeline
				auto bindingDescription = getVulkanVertexInputBindingDestription();
				auto attributeDescriptions = getVulkanVertexAttributeDescriptions();

				VkPipelineVertexInputStateCreateInfo vertexCreateInfo = {};
				vertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexCreateInfo.vertexBindingDescriptionCount = 1;
				vertexCreateInfo.pVertexBindingDescriptions = &bindingDescription;
				vertexCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				vertexCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

				VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
				inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

				//Create a viewport that covers the entire screen
				VkViewport viewport = {};
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)swapChain.extent.width;
				viewport.height = (float)swapChain.extent.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;

				//Render to the entire image
				VkRect2D scissor = {};
				scissor.offset = { 0, 0 };
				scissor.extent = swapChain.extent;

				VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
				viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportStateCreateInfo.pScissors = &scissor;
				viewportStateCreateInfo.scissorCount = 1;
				viewportStateCreateInfo.pViewports = &viewport;
				viewportStateCreateInfo.viewportCount = 1;

				VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {};
				rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
				rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
				rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
				rasterizationCreateInfo.depthBiasClamp = 0.0f;
				rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
				rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
				rasterizationCreateInfo.depthClampEnable = VK_FALSE;
				rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
				rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; //Change this to draw lines, dots etc
				rasterizationCreateInfo.lineWidth = 1.0f;

				//For enabling antialiasing based on multisampling
				VkPipelineMultisampleStateCreateInfo multisampleInfo = {};
				multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampleInfo.sampleShadingEnable = VK_FALSE;
				multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //Number of samples to use
				multisampleInfo.minSampleShading = 1.0f;
				multisampleInfo.pSampleMask = nullptr;
				multisampleInfo.alphaToCoverageEnable = VK_FALSE;
				multisampleInfo.alphaToOneEnable = VK_FALSE;

				VkPipelineDepthStencilStateCreateInfo depthCreateInfo = {};
				depthCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthCreateInfo.depthTestEnable = VK_TRUE;
				depthCreateInfo.depthWriteEnable = VK_TRUE;
				depthCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
				depthCreateInfo.depthBoundsTestEnable = VK_FALSE;
				depthCreateInfo.minDepthBounds = 0.0f;
				depthCreateInfo.maxDepthBounds = 1.0f;
				depthCreateInfo.stencilTestEnable = VK_FALSE;
				depthCreateInfo.front = {};
				depthCreateInfo.back = {};

				VkPipelineColorBlendAttachmentState colorBlendInfo = {};
				colorBlendInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				colorBlendInfo.blendEnable = VK_FALSE; //enable this if transparency should be enabled
				colorBlendInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				colorBlendInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				colorBlendInfo.alphaBlendOp = VK_BLEND_OP_ADD;
				colorBlendInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				colorBlendInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				colorBlendInfo.colorBlendOp = VK_BLEND_OP_ADD;

				VkPipelineColorBlendStateCreateInfo colorCreateInfo = {};
				colorCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorCreateInfo.attachmentCount = 1;
				colorCreateInfo.pAttachments = &colorBlendInfo;
				colorCreateInfo.logicOpEnable = VK_FALSE;

				//Make the viewport a dynamic state to allow for resizing
				VkDynamicState dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT };

				VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
				dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicStateCreateInfo.dynamicStateCount = 1;
				dynamicStateCreateInfo.pDynamicStates = &dynamicStates;

				VkPushConstantRange mainPushConstantRange = {};
				mainPushConstantRange.offset = 0;
				mainPushConstantRange.size = sizeof(MainPushConstant);
				mainPushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;

				std::array<VkPushConstantRange, 1> pushConstantRanges = { mainPushConstantRange };

				//Create the pipeline layout
				VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
				pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutCreateInfo.setLayoutCount = 1;
				pipelineLayoutCreateInfo.pSetLayouts = &pipeline.descriptorLayout;
				pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
				pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

				if (vkCreatePipelineLayout(devices.device, &pipelineLayoutCreateInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create pipeline render pipeline layout");
				}

				VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.stageCount = 2;
				pipelineCreateInfo.pStages = shaderStageCreationInfo;

				pipelineCreateInfo.pVertexInputState = &vertexCreateInfo;
				pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
				pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
				pipelineCreateInfo.pDynamicState = nullptr;
				pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
				pipelineCreateInfo.pMultisampleState = &multisampleInfo;
				pipelineCreateInfo.pDepthStencilState = &depthCreateInfo;
				pipelineCreateInfo.pColorBlendState = &colorCreateInfo;

				pipelineCreateInfo.layout = pipeline.pipelineLayout;

				pipelineCreateInfo.renderPass = swapChain.mainRenderPass;
				pipelineCreateInfo.subpass = 0;

				pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
				pipelineCreateInfo.basePipelineIndex = -1;

				if (vkCreateGraphicsPipelines(devices.device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline.renderPipeline) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create main graphics pipeline");
				}

				if (writeCache)
				{
					pipelineCacheFile.open("pipelinecache", std::ios::out | std::ios::binary);

					if (!pipelineCacheFile.is_open())
					{
						Logger::log("Error: Renderer could not open pipeline cache file for writing");
						return;
					}

					void* data;
					size_t dataSize;
					vkGetPipelineCacheData(devices.device, pipelineCache, &dataSize, nullptr);

					data = malloc(dataSize);

					vkGetPipelineCacheData(devices.device, pipelineCache, &dataSize, data);

					pipelineCacheFile.write(reinterpret_cast<const char*>(data), dataSize);

					pipelineCacheFile.close();

					Logger::log("Renderer: rebuilt and wrote pipeline cache");
				}
			}
		}
	}
}
#endif