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

		// Create test geometry
		testGeometry1 = Geometry::Create(vertices1, indices1);
		testGeometry2 = Geometry::Create(vertices2, indices2);
		
		return true;
	}

	void RenderAPI::OnResize()
	{
		m_rendererBackend->OnResize();
	}

	void RenderAPI::Shutdown() {
		delete testGeometry1;
		delete testGeometry2;
		m_rendererBackend->Shutdown();
	}
	
	bool RenderAPI::DrawFrame() {
		if (m_rendererBackend->BeginFrame()) {
			RendererGlobalState globalState = {};
			RendererGeometryData geometryData = {};
			m_rendererBackend->UpdateGlobalState(globalState);
			testGeometry1->Draw();
			testGeometry2->Draw();
			return m_rendererBackend->EndFrame();
		}
	}
}