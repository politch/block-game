#include "window.h"
#include "webgpu.h"

#include <GLFW/glfw3.h>
#include <atomic>

DEFINE_LOG_CATEGORY(Window);

static std::atomic<int> windowCount = 0;

Window::Window()
{
	LOG_CRITICAL_IF(Window, !glfwInit(), "Failed to initialize GLFW!");
	windowCount.fetch_add(1);
}

Window::~Window()
{
	Release();

	int count = windowCount.fetch_sub(1);
	if (count == 1) {
		glfwTerminate();
	}
}

void Window::Create(int width, int height, const char *title)
{
	Release();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void Window::Release()
{
	if (m_window) {
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose(m_window);
}

int Window::GetWidth() const
{
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	return width;
}

int Window::GetHeight() const
{
	int width, height;
	glfwGetWindowSize(m_window, &width, &height);
	return height;
}

void Window::PollEvents()
{
	glfwPollEvents();
}
