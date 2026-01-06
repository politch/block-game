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

	glfwSetWindowUserPointer(m_window, this);
	glfwSetKeyCallback(m_window, Window::KeyCallback);
	glfwSetMouseButtonCallback(m_window, Window::ButtonCallback);
	glfwSetCursorPosCallback(m_window, Window::CursorPosCallback);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

void Window::ResetInput()
{
	m_keyJustPressed.reset();
	m_keyJustReleased.reset();

	m_buttonJustPressed.reset();
	m_buttonJustReleased.reset();

	m_cursorDelta = { 0.0f, 0.0f };
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

void Window::KeyCallback(GLFWwindow *_window, int key, int scancode, int action,
			 int mods)
{
	Window *window =
		reinterpret_cast<Window *>(glfwGetWindowUserPointer(_window));

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (!window->m_keyPressed.test(key)) {
			window->m_keyPressed.set(key);
			window->m_keyJustPressed.set(key);
		}
	} else if (action == GLFW_RELEASE) {
		if (window->m_keyPressed.test(key)) {
			window->m_keyPressed.reset(key);
			window->m_keyJustReleased.set(key);
		}
	}
}

void Window::ButtonCallback(GLFWwindow *_window, int button, int action,
			    int mods)
{
	Window *window =
		reinterpret_cast<Window *>(glfwGetWindowUserPointer(_window));

	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (!window->m_buttonPressed.test(button)) {
			window->m_buttonPressed.set(button);
			window->m_buttonJustPressed.set(button);
		}
	} else if (action == GLFW_RELEASE) {
		if (window->m_buttonPressed.test(button)) {
			window->m_buttonPressed.reset(button);
			window->m_buttonJustReleased.set(button);
		}
	}
}

void Window::CursorPosCallback(GLFWwindow *_window, double xpos, double ypos)
{
	Window *window =
		reinterpret_cast<Window *>(glfwGetWindowUserPointer(_window));

	glm::vec2 cursorPos = { xpos, ypos };
	window->m_cursorDelta = cursorPos - window->m_cursorPos;
	window->m_cursorPos = cursorPos;
}
