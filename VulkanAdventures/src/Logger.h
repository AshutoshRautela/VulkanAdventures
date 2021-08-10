#pragma once

#pragma warning(push, 0)
#include <spdlog\spdlog.h>
#include <spdlog\sinks\stdout_color_sinks.h>
#pragma warning(pop)

#include <stdexcept>
#include <iostream>

namespace va {
	class Logger
	{
	public:
		static void Init();
		static void Info(...);
		static void Warn(...);
		static void Error(...);

		static std::shared_ptr<spdlog::logger> _oLogger;
	private:

	};
}

#define LOGGER_INFO(...)   ::va::Logger::_oLogger->info(__VA_ARGS__);
#define LOGGER_WARN(...)   ::va::Logger::_oLogger->warn(__VA_ARGS__);
#define LOGGER_ERROR(...)   ::va::Logger::_oLogger->error(__VA_ARGS__);
#define LOGGER_CRITICAL(...)   ::va::Logger::_oLogger->critical(__VA_ARGS__);

