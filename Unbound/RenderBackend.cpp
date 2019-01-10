#pragma once
#include "stdafx.h"
#include "RenderBackend.h"

namespace Unbound
{
	namespace Graphics
	{
		RenderBackendType RenderBackend::getType()
		{
			return backendType;
		}

		const uint32_t& RenderBackend::getNextFreeImageIndex() const
		{
			return temp;
		}

		RenderBackend::RenderBackend() noexcept
		{
			temp = 0;
			initialized = false;

#ifdef UNBOUND_BACKEND_OPENGL
			backendType = RenderBackendType::RENDER_BACKEND_OPENGL;
#elif defined UNBOUND_BACKEND_VULKAN
			backendType = RenderBackendType::RENDER_BACKEND_VULKAN;
#endif
		}

		RenderBackend::~RenderBackend()
		{
		}

		void RenderBackend::updateMatrices(const glm::mat4 & world, const glm::mat4 & view, const glm::mat4 & projection)
		{
		}

		void RenderBackend::updateWorldMatrix(const glm::mat4 & world)
		{
		}

		void RenderBackend::updateViewMatrix(const glm::mat4 & view)
		{
		}

		void RenderBackend::updateProjectionMatrix(const glm::mat4 & projection)
		{
		}
	}
}