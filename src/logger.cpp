#include "logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

DEFINE_LOG_CATEGORY(Default);

namespace __loggers__
{

std::shared_ptr<spdlog::logger> CreateLogger(const char *name)
{
	return spdlog::stdout_color_mt(name);
}

};
