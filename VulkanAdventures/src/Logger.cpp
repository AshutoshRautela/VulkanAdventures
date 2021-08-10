#include "Logger.h"

namespace va {

	std::shared_ptr<spdlog::logger> Logger::_oLogger;

	void Logger::Init() {
		try {
			Logger::_oLogger = spdlog::stdout_color_mt("VA");
			Logger::_oLogger->set_level(spdlog::level::trace);
		}
		catch (std::exception e) {
			spdlog::critical("Failed to create a logger");
		}
	}
}