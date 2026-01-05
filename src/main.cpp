#include <iostream>
#include <GLFW/glfw3.h>

#include <webgpu/webgpu_glfw.h>
#include <webgpu/webgpu_cpp.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_cpp_print.h>
#endif

#if defined(__EMSCRIPTEN__)
std::ostream &operator<<(std::ostream &os, wgpu::ErrorType error)
{
	switch (error) {
	case wgpu::ErrorType::NoError:
		os << "NoError";
		break;
	case wgpu::ErrorType::Internal:
		os << "Internal";
		break;
	case wgpu::ErrorType::OutOfMemory:
		os << "OutOfMemory";
		break;
	case wgpu::ErrorType::Validation:
		os << "Validation";
		break;
	case wgpu::ErrorType::Unknown:
		os << "Unknown";
		break;
	}

	return os;
}

std::ostream &operator<<(std::ostream &os, wgpu::StringView view)
{
	os.write(view.data, view.length);
	return os;
}
#endif

const uint32_t kWidth = 512;
const uint32_t kHeight = 512;

wgpu::Instance instance;
wgpu::Adapter adapter;
wgpu::Device device;
wgpu::Surface surface;
wgpu::TextureFormat format;
wgpu::RenderPipeline pipeline;

GLFWwindow *window;

const char shaderCode[] = R"(
@vertex fn vertexMain(@builtin(vertex_index) i : u32) -> @builtin(position) vec4f {
  const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
  return vec4f(pos[i], 0, 1);
}
@fragment fn fragmentMain() -> @location(0) vec4f {
  return vec4f(1, 0, 0, 1);
}
)";

void Init()
{
	static const auto kTimedWaitAny =
		wgpu::InstanceFeatureName::TimedWaitAny;

	wgpu::InstanceDescriptor instanceDesc = {
		.requiredFeatureCount = 1, .requiredFeatures = &kTimedWaitAny
	};

	instance = wgpu::CreateInstance(&instanceDesc);

	wgpu::Future f1 = instance.RequestAdapter(
		nullptr, wgpu::CallbackMode::WaitAnyOnly,
		[](wgpu::RequestAdapterStatus status, wgpu::Adapter a,
		   wgpu::StringView message) {
			if (status != wgpu::RequestAdapterStatus::Success) {
				std::cerr << "RequestAdapter: " << message
					  << "\n";
				exit(0);
			}

			adapter = std::move(a);
		});

	instance.WaitAny(f1, UINT64_MAX);

	wgpu::DeviceDescriptor deviceDesc = {};
	deviceDesc.SetUncapturedErrorCallback([](const wgpu::Device &,
						 wgpu::ErrorType errorType,
						 wgpu::StringView message) {
		std::cerr << "Error: " << errorType << " - message: " << message
			  << "\n";
	});

	wgpu::Future f2 = adapter.RequestDevice(
		nullptr, wgpu::CallbackMode::WaitAnyOnly,
		[](wgpu::RequestDeviceStatus status, wgpu::Device d,
		   wgpu::StringView message) {
			if (status != wgpu::RequestDeviceStatus::Success) {
				std::cerr << "RequestDevice: " << message
					  << "\n";
				exit(0);
			}

			device = std::move(d);
		});

	instance.WaitAny(f2, UINT64_MAX);
}

void InitWindow()
{
	if (!glfwInit()) {
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(kWidth, kHeight, "WebGPU window", nullptr,
				  nullptr);
}

void InitSurface()
{
	surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);

	wgpu::SurfaceCapabilities capabilities;
	surface.GetCapabilities(adapter, &capabilities);
	format = capabilities.formats[0];

	wgpu::SurfaceConfiguration config = {
		.device = device,
		.format = format,
		.width = kWidth,
		.height = kHeight,
	};

	surface.Configure(&config);
}

void InitRenderPipeline()
{
	wgpu::ShaderSourceWGSL wgsl({ .code = shaderCode });

	wgpu::ShaderModuleDescriptor shaderModuleDesc = { .nextInChain =
								  &wgsl };

	wgpu::ShaderModule module =
		device.CreateShaderModule(&shaderModuleDesc);

	wgpu::ColorTargetState colorTargetState = { .format = format };

	wgpu::FragmentState fragmentState = { .module = module,
					      .targetCount = 1,
					      .targets = &colorTargetState };

	wgpu::RenderPipelineDescriptor desc = { .vertex = { .module = module },
						.fragment = &fragmentState };

	pipeline = device.CreateRenderPipeline(&desc);
}

void Render()
{
	wgpu::SurfaceTexture surfaceTexture;
	surface.GetCurrentTexture(&surfaceTexture);

	wgpu::RenderPassColorAttachment attachment = {
		.view = surfaceTexture.texture.CreateView(),
		.loadOp = wgpu::LoadOp::Clear,
		.storeOp = wgpu::StoreOp::Store,
	};

	wgpu::RenderPassDescriptor renderPassDesc = {
		.colorAttachmentCount = 1,
		.colorAttachments = &attachment,
	};

	wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
	wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);

	pass.SetPipeline(pipeline);
	pass.Draw(3);
	pass.End();

	wgpu::CommandBuffer commands = encoder.Finish();
	device.GetQueue().Submit(1, &commands);
}

void Run()
{
#if defined(__EMSCRIPTEN__)
	emscripten_set_main_loop(Render, 0, false);
#else
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Render();
		surface.Present();
		instance.ProcessEvents();
	}
#endif
}

int main()
{
	Init();
	InitWindow();
	InitSurface();
	InitRenderPipeline();

	Run();
}
