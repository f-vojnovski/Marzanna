#pragma once

namespace mz {
	class Application {
	public:
		Application();
		virtual ~Application();
		bool Run();
		void Shutdown();
	private:
		bool m_isRunning = true;
		bool m_isSuspended = false;
	};

	// Defined in client
	extern Application* CreateApplication();
}