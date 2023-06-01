#include "application.h"

namespace mz {
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application::Application()
	{
		MZ_CORE_INFO("Running Marzanna engine...");
	
		MZ_CORE_TRACE("Creating window...");
		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}
	Application::~Application()
	{
	}
	bool Application::Run()
	{


		while (m_isRunning) {
			m_window->OnUpdate();
		}

		return true;
	}
	void Application::Shutdown()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		//dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		//dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		// TODO: Perhaps log event here?
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_isRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		
		return false;
	}
}