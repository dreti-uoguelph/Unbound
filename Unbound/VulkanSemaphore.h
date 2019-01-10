#pragma
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
			void createSemaphores(Devices& devices, Semaphores& toCreate);

			//Destroy all the semaphores that were created
			void destroySemaphores(Devices& devices, Semaphores& semaphores);
		}
	}
}
#endif