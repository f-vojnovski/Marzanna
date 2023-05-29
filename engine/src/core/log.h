#pragma once

#include <memory>

#include <spdlog/spdlog.h>

namespace mz {
	class Log {
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_clientLogger; }
		static void ReportAssertionFailure(const char* expression, const char* message, const char* file, int line);

	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

// Engine log macros
#define MZ_CORE_TRACE(...)    ::mz::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define MZ_CORE_INFO(...)     ::mz::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MZ_CORE_WARN(...)     ::mz::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MZ_CORE_ERROR(...)    ::mz::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MZ_CORE_CRITICAL(...) ::mz::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define MZ_TRACE(...)	      ::mz::Log::GetClientLogger()->trace(__VA_ARGS__)
#define MZ_INFO(...)	      ::mz::Log::GetClientLogger()->info(__VA_ARGS__)
#define MZ_WARN(...)	      ::mz::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MZ_ERROR(...)	      ::mz::Log::GetClientLogger()->error(__VA_ARGS__)
#define MZ_CRITICAL(...)      ::mz::Log::GetClientLogger()->critical(__VA_ARGS__) 