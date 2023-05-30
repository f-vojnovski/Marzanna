#include "application.h"

namespace mz {
	Application::Application()
	{
	}
	Application::~Application()
	{
	}
	bool Application::Run()
	{
		mz::Log::GetCoreLogger()->trace("Running Marzanna engine...");

		while (m_isRunning) {
		}

		return true;
	}
	void Application::Shutdown()
	{
	}
}