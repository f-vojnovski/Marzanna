#pragma once

#include "engine/src/mzpch.h"
#include "events/application_event.h"
#include "engine/src/platform/windows_window.h"

namespace mz {
	class Application {
	public:
		Application();
		virtual ~Application();
		bool Run();
		void Shutdown();

		void OnEvent(Event& e);
	private:
		bool m_isRunning = true;
		bool m_isSuspended = false;

		std::unique_ptr<Window> m_window;
		int16_t m_width;
		int16_t m_height;

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	};

	// Defined in client
	extern Application* CreateApplication();
}