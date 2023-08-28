#include "application.h"
#include "engine/src/asserts.h"
#include "engine/src/renderer/render_api.h"

namespace mz {
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		MZ_CORE_INFO("Running Marzanna engine...");	

		MZ_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		RenderApiArgs args = { "Marzanna application", m_window.get() };
		m_renderApi = std::make_unique<RenderAPI>(args);

		m_renderApi->Initialize();

		m_geometrySystem = std::make_unique<GeometrySystem>();

		m_activeScene = std::make_unique<Scene>();
	}

	Application::~Application()
	{
	}

	bool Application::Run()
	{
		while (m_isRunning) {
			if (m_isSuspended) continue;

			m_window->OnUpdate();
			m_activeScene->OnGraphicsUpdate();

			for (Layer* layer : m_layerStack) {
				layer->OnUpdate();
			}
		}

		Shutdown();
		return true;
	}
	void Application::Shutdown()
	{
		m_geometrySystem->Shutdown();
		m_renderApi->Shutdown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		
		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}

		// Uncomment to log events 
		// MZ_CORE_TRACE("{0}", e.ToString());
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_isRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) 
	{
		m_renderApi->OnResize();
		return true;
	}
	
	void Application::PushLayer(Layer* layer)
	{
		m_layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
}