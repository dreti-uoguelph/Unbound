#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanStructs.h"
#include"VulkanImage.h"
#include<array>
#include"ImageLoader.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Updates the descriptor sets to handle new image bindings
			void updateDescriptorSets(Devices& devices, Pipeline& pipeline, Buffers& buffers);
			//Create the descriptor pool for allocating desciptor buffers
			void createDescriptorPool(Devices& devices, Buffers& buffers, Pipeline& pipeline);
			//Create the descriptor sets required for sending uniforms to shaders
			void createDescriptorSets(Devices& devices, Pipeline& pipeline, SwapChain& swapChain, Buffers& buffers);
			//Create the descriptor set layout for setting uniforms in vulkan
			void createDescriptorSetLayout(Devices& devices, Buffers& buffers, Pipeline& pipeline);
		}
	}
}
#endif