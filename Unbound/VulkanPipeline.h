#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"Shader.h"
#include"Vertex.h"
#include"VulkanShader.h"
#include"VulkanStructs.h"
#include<fstream>

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Initialize the graphics pipeline, preparing it for rendering
			void initializeGraphicsPipeline(Devices& devices, Pipeline& pipeline, SwapChain& swapChain);
		}
	}
}
#endif