#pragma once

#include "logger.h"

#include <sstream>

#include <GLFW/glfw3.h>
#include <webgpu/webgpu_glfw.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <webgpu/webgpu_cpp.h>
#else
#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#endif

#if defined(__EMSCRIPTEN__)
std::ostream &operator<<(std::ostream &os, wgpu::ErrorType error);
std::ostream &operator<<(std::ostream &os, wgpu::StringView view);
#endif

template <>
struct fmt::formatter<wgpu::ErrorType> : fmt::formatter<std::string> {
	auto format(wgpu::ErrorType error, format_context &ctx) const
		-> decltype(ctx.out())
	{
		std::ostringstream ss;
		ss << error;
		return fmt::format_to(ctx.out(), "{}", ss.str());
	}
};

template <>
struct fmt::formatter<wgpu::StringView> : fmt::formatter<std::string> {
	auto format(wgpu::StringView view, format_context &ctx) const
		-> decltype(ctx.out())
	{
		std::ostringstream ss;
		ss << view;
		return fmt::format_to(ctx.out(), "{}", ss.str());
	}
};
