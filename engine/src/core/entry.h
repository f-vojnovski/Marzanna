#pragma once

#include "application.h"
#include "log.h"

int main(int argc, char** argv) {
	mz::Log::Init();
	mz::Application* app = new mz::Application();

	app->Run();
	delete app;

	return 0;
}

extern mz::Application* mz::CreateApplication();
