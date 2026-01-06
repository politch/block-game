#pragma once

class Config {
    public:
	static Config &Get();

	inline void SetWidth(int width)
	{
		m_width = width;
	}

	inline int GetWidth() const
	{
		return m_width;
	}

	inline void SetHeight(int height)
	{
		m_height = height;
	}

	inline int GetHeight() const
	{
		return m_height;
	}

	inline void SetTitle(const char *title)
	{
		m_title = title;
	}

	inline const char *GetTitle()
	{
		return m_title;
	}

    private:
	Config()
	{
	}

	Config(const Config &other) = delete;
	Config(Config &&other) = delete;

	Config &operator=(const Config &other) = delete;
	Config &operator=(Config &&other) = delete;

    private:
	int m_width;
	int m_height;
	const char *m_title;
};
