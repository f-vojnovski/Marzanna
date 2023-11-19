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

		// Create a perspective camera
		testCamera = new PerspectiveCamera(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

		return true;
	}

	void RenderAPI::OnResize()
	{
		m_rendererBackend->OnResize();
	}

	void RenderAPI::Shutdown() {
		m_rendererBackend->Shutdown();
	}
	
	bool RenderAPI::DrawFrame(RenderApiDrawCallArgs args) {
		if (m_rendererBackend->BeginFrame()) {
			RendererGlobalState globalState = {};
			RendererGeometryData geometryData = {};

			globalState.projection = testCamera->GetProjectionMatrix();
			globalState.view = testCamera->GetViewMatrix();

			for (GeometryWithPosition geometryToDraw : args.geometries) {
				geometryToDraw.geometry->Draw(geometryToDraw.model);
			}

			m_rendererBackend->UpdateGlobalState(globalState);
			return m_rendererBackend->EndFrame();
		}
	}
}