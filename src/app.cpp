#include "app.h"

#include "config.h"

DEFINE_LOG_CATEGORY(Application);
DEFINE_LOG_CATEGORY(WebGPU);

#if defined(__EMSCRIPTEN__)
static Application *g_instance = nullptr;
#endif

void Application::Create()
{
	InitWindow();
	InitInstance();
	InitAdapter();
	InitDevice();
	InitSurface();

	Init();
}

void Application::Loop()
{
	Update();
	Render();
}

void Application::Release()
{
	Destroy();

	if (m_surface) {
		m_surface.Unconfigure();
		m_surface = nullptr;
	}

	if (m_device) {
		m_device = nullptr;
	}

	if (m_adapter) {
		m_adapter = nullptr;
	}

	if (m_instance) {
		m_instance = nullptr;
	}

	m_window.Release();
}

void Application::EmscriptenLoop()
{
	g_instance->Loop();
}

void Application::Run()
{
	Create();

#if defined(__EMSCRIPTEN__)
	LOG_CRITICAL_IF(Application, g_instance != nullptr,
			"Application is already running!");

	g_instance = this;
	atexit([] {
		g_instance->Release();
		g_instance = nullptr;
	});

	emscripten_set_main_loop(Application::EmscriptenLoop, 0, true);
#else
	while (!m_window.ShouldClose()) {
		Window::PollEvents();
		Loop();
		m_surface.Present();
		m_instance.ProcessEvents();
	}

	Release();
#endif
}

void Application::InitWindow()
{
	auto &config = Config::Get();
	int width = config.GetWidth();
	int height = config.GetHeight();
	const char *title = config.GetTitle();
	m_window.Create(width, height, title);
}

void Application::InitInstance()
{
	static const auto kTimedWaitAny =
		wgpu::InstanceFeatureName::TimedWaitAny;

	wgpu::InstanceDescriptor instanceDesc = {
		.requiredFeatureCount = 1,
		.requiredFeatures = &kTimedWaitAny,
	};

	m_instance = wgpu::CreateInstance(&instanceDesc);
}

void Application::InitAdapter()
{
	wgpu::Future future = m_instance.RequestAdapter(
		nullptr, wgpu::CallbackMode::WaitAnyOnly,
		[this](wgpu::RequestAdapterStatus status, wgpu::Adapter a,
		       wgpu::StringView message) {
			LOG_CRITICAL_IF(
				WebGPU,
				status != wgpu::RequestAdapterStatus::Success,
				"RequestAdapter: {}", message);

			m_adapter = std::move(a);
		});

	m_instance.WaitAny(future, UINT64_MAX);
}

void Application::InitDevice()
{
	wgpu::DeviceDescriptor deviceDesc = {};
	deviceDesc.SetUncapturedErrorCallback([](const wgpu::Device &,
						 wgpu::ErrorType errorType,
						 wgpu::StringView message) {
		LOG_ERROR(WebGPU, "Error: {} - message: {}", errorType,
			  message);
	});

	wgpu::Future future = m_adapter.RequestDevice(
		nullptr, wgpu::CallbackMode::WaitAnyOnly,
		[this](wgpu::RequestDeviceStatus status, wgpu::Device d,
		       wgpu::StringView message) {
			LOG_CRITICAL_IF(
				WebGPU,
				status != wgpu::RequestDeviceStatus::Success,
				"RequestDevice: {}", message);

			m_device = std::move(d);
		});

	m_instance.WaitAny(future, UINT64_MAX);
}

void Application::InitSurface()
{
	m_surface = wgpu::glfw::CreateSurfaceForWindow(m_instance,
						       m_window.GetHandle());

	wgpu::SurfaceCapabilities capabilities;
	m_surface.GetCapabilities(m_adapter, &capabilities);
	m_format = capabilities.formats[0];

	wgpu::SurfaceConfiguration config = {
		.device = m_device,
		.format = m_format,
		.width = static_cast<uint32_t>(m_window.GetWidth()),
		.height = static_cast<uint32_t>(m_window.GetHeight()),
	};

	m_surface.Configure(&config);
}

wgpu::RenderPipeline Application::CreateRenderPipeline(const char *src)
{
	wgpu::ShaderSourceWGSL wgsl({
		.code = src,
	});

	wgpu::ShaderModuleDescriptor shaderModuleDesc = {
		.nextInChain = &wgsl,
	};

	wgpu::ShaderModule module =
		m_device.CreateShaderModule(&shaderModuleDesc);

	wgpu::BlendState blendState = {
    .color = {
      .operation = wgpu::BlendOperation::Add,
      .srcFactor = wgpu::BlendFactor::SrcAlpha,
      .dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha,
    },
    .alpha = {
      .operation = wgpu::BlendOperation::Add,
      .srcFactor = wgpu::BlendFactor::Zero,
      .dstFactor = wgpu::BlendFactor::One,
    },
	};

	wgpu::ColorTargetState colorTargetState = {
		.format = m_format,
		.blend = &blendState,
		.writeMask = wgpu::ColorWriteMask::All,
	};

	wgpu::FragmentState fragmentState = {
		.module = module,
		.entryPoint = "fs_main",
		.constantCount = 0,
		.constants = nullptr,
		.targetCount = 1,
		.targets = &colorTargetState,
	};

	wgpu::RenderPipelineDescriptor desc = {
    .layout = nullptr,
		.vertex = {
      .module = module,
      .entryPoint = "vs_main",
      .constantCount = 0,
      .constants = nullptr,
      .bufferCount = 0,
      .buffers = nullptr,
    },
		.primitive = {
      .topology = wgpu::PrimitiveTopology::TriangleList,
      .stripIndexFormat = wgpu::IndexFormat::Undefined,
      .frontFace = wgpu::FrontFace::CCW,
      .cullMode = wgpu::CullMode::None
    },
    .depthStencil = nullptr,
    .multisample = {
      .count = 1,
      .mask = ~0u,
      .alphaToCoverageEnabled = false,
    },
    .fragment = &fragmentState,
	};

	return m_device.CreateRenderPipeline(&desc);
}
