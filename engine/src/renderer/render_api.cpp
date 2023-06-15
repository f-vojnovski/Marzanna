#include "render_api.h"
#include "vulkan/vulkan_renderer_backend.h"
#include "engine/src/core/log.h"

namespace mz {
	RenderAPI::RenderAPI(const RenderApiArgs args) {
		// Use factory here if multiple renderer types
		
		RendererBackendArgs backendArgs;
		backendArgs.name = args.name;
		backendArgs.window = args.window;
		m_rendererBackend = std::make_unique<VulkanRendererBackend>(backendArgs);
	}

	bool RenderAPI::Initialize() {
		MZ_CORE_TRACE("Creating renderer backend...");
		if (!m_rendererBackend->Initialize()) {
			MZ_CORE_CRITICAL("Renderer backend creation failed!");
			return false;
		}
		MZ_CORE_INFO("Created renderer backend!");
		return true;
	}

	void RenderAPI::Shutdown() {
		m_rendererBackend->Shutdown();
	}
	
	bool RenderAPI::DrawFrame() {
		if (m_rendererBackend->BeginFrame()) {
			RendererGlobalState globalState = {};
			RendererGeometryData geometryData = {};
			m_rendererBackend->UpdateGlobalState(globalState);
			m_rendererBackend->DrawGeometries(geometryData);
			return m_rendererBackend->EndFrame();
		}
	}
}