#include "application.h"

namespace mz {
	Application::Application()
	{
		MZ_CORE_INFO("Running Marzanna engine...");
	}
	Application::~Application()
	{
	}
	bool Application::Run()
	{

		while (m_isRunning) {
		}

		return true;
	}
	void Application::Shutdown()
	{
	}
}