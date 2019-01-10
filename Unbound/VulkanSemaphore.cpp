#pragma once
#include"stdafx.h"
#ifdef UNBOUND_BACKEND_VULKAN
#include"VulkanStructs.h"

namespace Unbound
{
	namespace Graphics
	{
		namespace Vulkan
		{
			//Create semaphores to signal different stages of rendering
			void createSemaphores(Devices& devices, Semaphores& toCreate)
			{
				VkSemaphoreCreateInfo semaphoreInfo = {};
				semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

				if (vkCreateSemaphore(devices.device, &semaphoreInfo, nullptr, &toCreate.imageAcquiredSemaphore) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create semaphores for drawing operations");
				}

				if (vkCreateSemaphore(devices.device, &semaphoreInfo, nullptr, &toCreate.renderFinishedSemaphore) != VK_SUCCESS)
				{
					throw std::runtime_error("Could not create semaphores for drawing operations");
				}
			}

			//Destroy all the semaphores that were created earlier
			void destroySemaphores(Devices& devices, Semaphores& semaphores)
			{
				vkDestroySemaphore(devices.device, semaphores.imageAcquiredSemaphore, nullptr);
				vkDestroySemaphore(devices.device, semaphores.renderFinishedSemaphore, nullptr);
			}
		}
	}
}
#endif