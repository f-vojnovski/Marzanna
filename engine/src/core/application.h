#pragma once

#include "engine/src/mzpch.h"
#include "events/application_event.h"
#include "engine/src/platform/windows_window.h"
#include "engine/src/core/layer_stack.h"
#include "engine/src/renderer/render_api.h"
#include "engine/src/system/scene/scene.h"
#include "engine/src/system/texture_system.h"

namespace mz {
	class Application {
	public:
		Application();
		virtual ~Application();
		bool Run();
		void Shutdown();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_window; }
		inline RenderApiType GetRenderApiType() { return m_renderApi->GetType(); }
		inline RenderAPI& GetRenderApi() { return *m_renderApi; }
		
		std::shared_ptr<Scene> m_activeScene;

	protected:
		std::unique_ptr<GeometrySystem> m_geometrySystem;

	private:
		bool m_isRunning = true;
		bool m_isSuspended = false;

		std::unique_ptr<Window> m_window;
		int16_t m_width;
		int16_t m_height;

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	
		LayerStack m_layerStack;

		std::unique_ptr<RenderAPI> m_renderApi;
		std::unique_ptr<TextureSystem> m_textureSystem;

		static Application* s_Instance;
	};

	// Defined in client
	extern Application* CreateApplication();
}