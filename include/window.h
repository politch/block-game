#pragma once

#include "logger.h"
#include "webgpu.h"

#include <glm/glm.hpp>

#include <bitset>

DECLARE_LOG_CATEGORY(Window);

class Window {
    public:
	Window();
	~Window();

	void Create(int width, int height, const char *title);
	void Release();

	bool ShouldClose();
	void ResetInput();

	int GetWidth() const;
	int GetHeight() const;

	inline bool IsKeyPressed(int key) const
	{
		return m_keyPressed.test(key);
	}

	inline bool IsKeyJustPressed(int key) const
	{
		return m_keyJustPressed.test(key);
	}

	inline bool IsKeyJustReleased(int key) const
	{
		return m_keyJustReleased.test(key);
	}

	inline bool IsButtonPressed(int button) const
	{
		return m_buttonPressed.test(button);
	}

	inline bool IsButtonJustPressed(int button) const
	{
		return m_buttonJustPressed.test(button);
	}

	inline bool IsButtonJustReleased(int button) const
	{
		return m_buttonJustReleased.test(button);
	}

	inline glm::vec2 GetCursorPos() const
	{
		return m_cursorPos;
	}

	inline glm::vec2 GetCursorDelta() const
	{
		return m_cursorDelta;
	}

	inline GLFWwindow *GetHandle() const
	{
		return m_window;
	}

	static void PollEvents();

    private:
	static void KeyCallback(GLFWwindow *window, int key, int scancode,
				int action, int mods);

	static void ButtonCallback(GLFWwindow *window, int button, int action,
				   int mods);

	static void CursorPosCallback(GLFWwindow *window, double xpos,
				      double ypos);

    private:
	GLFWwindow *m_window;

	glm::vec2 m_cursorPos;
	glm::vec2 m_cursorDelta;

	std::bitset<GLFW_KEY_LAST + 1> m_keyPressed;
	std::bitset<GLFW_KEY_LAST + 1> m_keyJustPressed;
	std::bitset<GLFW_KEY_LAST + 1> m_keyJustReleased;

	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> m_buttonPressed;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> m_buttonJustPressed;
	std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> m_buttonJustReleased;
};
