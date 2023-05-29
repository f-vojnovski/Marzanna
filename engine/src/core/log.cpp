#include "log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace mz {
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_coreLogger = spdlog::stdout_color_mt("ENGINE");
		s_coreLogger->set_level(spdlog::level::trace);

		s_clientLogger = spdlog::stdout_color_mt("APPLICATION");
		s_clientLogger->set_level(spdlog::level::trace);
	}

	void Log::ReportAssertionFailure(const char* expression, const char* message, const char* file, int line) {
		MZ_CORE_ERROR("Assertion failure: %s, message: %s, in file: %s, in line: %s", expression, message, file, line);
	}
}

