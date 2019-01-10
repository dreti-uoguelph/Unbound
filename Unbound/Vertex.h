#pragma once
#include"stdafx.h"
#include<array>

namespace Unbound
{
	namespace Graphics
	{
		/*Models are made of vertices. Each one is 64 bytes in size for alignment*/
		struct Vertex
		{
			glm::vec3 position; //The position of the vertex
			glm::vec3 normal; //The normal vector of the vertex
			glm::vec3 tangent; //The tangent of the vertex, used for normal mapping
			glm::vec3 color; //The color of the vertex, can be used for certain effects
			glm::vec2 uv; //The first set of UV coordinates, used for texturing
			glm::vec2 uv2; //Secondary UV coordinates, used for terrain heighmapping and other special effects
		};

#ifdef UNBOUND_BACKEND_VULKAN
		inline VkVertexInputBindingDescription getVulkanVertexInputBindingDestription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex); //Should be 64 bytes
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		inline std::array<VkVertexInputAttributeDescription, 6> getVulkanVertexAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 6> attributeDescriptions = {};

			//The first 4 attributes are of type glm::vec3, so they will have matching attributes
			for (int i = 0; i < 4; i++)
			{
				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i; //Refereces the location in vertex shader
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[i].offset = i * sizeof(glm::vec3);
			}

			//The last 2 attributes have a smaller size so their bindings must be filled out accordingly
			for (int i = 4; i < 6; i++)
			{
				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[i].offset = (4 * sizeof(glm::vec3)) + ((i - 4) * sizeof(glm::vec2));
			}

			return attributeDescriptions;
		}
#endif
	}
}