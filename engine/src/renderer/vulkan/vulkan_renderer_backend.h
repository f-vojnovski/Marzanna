#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/renderer_backend.h"

namespace mz {
	class VulkanRendererBackend : public RendererBackend {
	public:
		VulkanRendererBackend(const RendererBackendArgs args);
		virtual bool Initialize() override;
		virtual void Shutdown() override;
		virtual bool BeginFrame() override;
		virtual bool EndFrame() override;
	};
}