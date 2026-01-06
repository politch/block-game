#include "app.h"
#include "config.h"
#include "entrypoint.h"

const char shaderCode[] = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
  var p = vec2f(0.0, 0.0);
  if (in_vertex_index == 0u) {
    p = vec2f(-0.5, -0.5);
  } else if (in_vertex_index == 1u) {
    p = vec2f(0.5, -0.5);
  } else {
    p = vec2f(0.0, 0.5);
  }
  return vec4f(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
  return vec4f(1.0, 0.4, 1.0, 1.0);
}
)";

class BlockGameApplication : public Application {
    public:
	virtual void Init() override
	{
		m_pipeline = CreateRenderPipeline(shaderCode);
	}

	virtual void Render() override
	{
		wgpu::Device &device = GetDevice();
		wgpu::Surface &surface = GetSurface();

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
		wgpu::RenderPassEncoder pass =
			encoder.BeginRenderPass(&renderPassDesc);

		pass.SetPipeline(m_pipeline);
		pass.Draw(3);
		pass.End();

		wgpu::CommandBuffer commands = encoder.Finish();
		device.GetQueue().Submit(1, &commands);
	}

	virtual void Update() override
	{
	}

	virtual void Destroy() override
	{
		if (m_pipeline) {
			m_pipeline = nullptr;
		}
	}

    private:
	wgpu::RenderPipeline m_pipeline;
};

std::unique_ptr<Application> CreateApplication()
{
	Config &config = Config::Get();
	config.SetWidth(512);
	config.SetHeight(512);
	config.SetTitle("Block Game");

	return std::make_unique<BlockGameApplication>();
}
