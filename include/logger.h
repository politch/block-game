#pragma once

#ifndef LOG_ACTIVE_LEVEL
#define LOG_ACTIVE_LEVEL LOG_LEVEL_INFO
#endif

#define LOG_LEVEL_TRACE SPDLOG_LEVEL_TRACE
#define LOG_LEVEL_DEBUG SPDLOG_LEVEL_DEBUG
#define LOG_LEVEL_INFO SPDLOG_LEVEL_INFO
#define LOG_LEVEL_WARN SPDLOG_LEVEL_WARN
#define LOG_LEVEL_ERROR SPDLOG_LEVEL_ERROR
#define LOG_LEVEL_CRITICAL SPDLOG_LEVEL_CRITICAL

#define SPDLOG_ACTIVE_LEVEL LOG_ACTIVE_LEVEL
#include <spdlog/spdlog.h>

#define DECLARE_LOG_CATEGORY(category)                               \
	namespace __loggers__                                        \
	{                                                            \
	extern std::shared_ptr<spdlog::logger> __logger__##category; \
	};

#define DEFINE_LOG_CATEGORY(category)                          \
	namespace __loggers__                                  \
	{                                                      \
	std::shared_ptr<spdlog::logger> __logger__##category = \
		CreateLogger(#category);                       \
	};

#define LOGGER(category) __loggers__::__logger__##category

#define LOG_TRACE(category, ...) \
	SPDLOG_LOGGER_TRACE(LOGGER(category), __VA_ARGS__)

#define LOG_DEBUG(category, ...) \
	SPDLOG_LOGGER_DEBUG(LOGGER(category), __VA_ARGS__)

#define LOG_INFO(category, ...) \
	SPDLOG_LOGGER_INFO(LOGGER(category), __VA_ARGS__)

#define LOG_WARN(category, ...) \
	SPDLOG_LOGGER_WARN(LOGGER(category), __VA_ARGS__)

#define LOG_ERROR(category, ...) \
	SPDLOG_LOGGER_ERROR(LOGGER(category), __VA_ARGS__)

#define LOG_CRITICAL(category, ...) \
	SPDLOG_LOGGER_CRITICAL(LOGGER(category), __VA_ARGS__)

#define LOG_TRACE_IF(category, condition, ...)    \
	if (condition) {                          \
		LOG_TRACE(category, __VA_ARGS__); \
	}

#define LOG_DEBUG_IF(category, condition, ...)    \
	if (condition) {                          \
		LOG_DEBUG(category, __VA_ARGS__); \
	}

#define LOG_INFO_IF(category, condition, ...)    \
	if (condition) {                         \
		LOG_INFO(category, __VA_ARGS__); \
	}

#define LOG_WARN_IF(category, condition, ...)    \
	if (condition) {                         \
		LOG_WARN(category, __VA_ARGS__); \
	}

#define LOG_ERROR_IF(category, condition, ...)    \
	if (condition) {                          \
		LOG_ERROR(category, __VA_ARGS__); \
	}

#define LOG_CRITICAL_IF(category, condition, ...)    \
	if (condition) {                             \
		LOG_CRITICAL(category, __VA_ARGS__); \
	}

DECLARE_LOG_CATEGORY(Default);

namespace __loggers__
{

std::shared_ptr<spdlog::logger> CreateLogger(const char *name);

};
