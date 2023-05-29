#include <iostream>
#include "core/log.h"

int main() {
	mz::Log::Init();
	mz::Log::GetCoreLogger()->trace("Trace test");
	mz::Log::GetCoreLogger()->info("Info test");
	mz::Log::GetCoreLogger()->warn("Warning test");
	mz::Log::GetCoreLogger()->error("Error test");
	mz::Log::GetCoreLogger()->critical("Critical test");
}