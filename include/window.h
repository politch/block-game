#pragma once

#include "logger.h"

struct GLFWwindow;

DECLARE_LOG_CATEGORY(Window);

class Window {
    public:
	Window();
	~Window();

	void Create(int width, int height, const char *title);
	void Release();

	bool ShouldClose();

	int GetWidth() const;
	int GetHeight() const;

	inline GLFWwindow *GetHandle() const
	{
		return m_window;
	}

	static void PollEvents();

    private:
	GLFWwindow *m_window;
};
