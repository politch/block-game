#pragma once

#include "logger.h"
#include "webgpu.h"
#include "window.h"

DECLARE_LOG_CATEGORY(WebGPU);

class Application {
    public:
	Application() = default;
	~Application() = default;

	virtual void Init()
	{
	}
	virtual void Render()
	{
	}
	virtual void Update()
	{
	}
	virtual void Destroy()
	{
	}

	void Run();

	wgpu::RenderPipeline CreateRenderPipeline(const char *src);

	inline wgpu::Instance &GetInstance()
	{
		return m_instance;
	}

	inline wgpu::Adapter &GetAdapter()
	{
		return m_adapter;
	}

	inline wgpu::Device &GetDevice()
	{
		return m_device;
	}

	inline wgpu::Surface &GetSurface()
	{
		return m_surface;
	}

    private:
	void InitWindow();
	void InitInstance();
	void InitAdapter();
	void InitDevice();
	void InitSurface();

	void Create();
	void Loop();
	void Release();

	static void EmscriptenLoop();

    private:
	Window m_window;
	wgpu::Instance m_instance;
	wgpu::Adapter m_adapter;
	wgpu::Device m_device;
	wgpu::Surface m_surface;
	wgpu::TextureFormat m_format;
};
