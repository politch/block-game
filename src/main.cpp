#include "app.h"
#include "config.h"
#include "entrypoint.h"

#include "pipeline.h"
#include "ssbo.h"

#include "uniform.h"
#include "webgpu/webgpu_cpp.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WORLD_SIZE 32

// clang-format off
static std::vector<float> vertexData({
    // Front face (Z+)
    // Triangle 1
    -0.5f, -0.5f,  0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f,  0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f,  0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    // Triangle 2
     0.5f,  0.5f,  0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f,  0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.5f, -0.5f,  0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Back face (Z-)
    // Triangle 1
     0.5f, -0.5f, -0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left (from back view)
    -0.5f, -0.5f, -0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right (from back view)
    -0.5f,  0.5f, -0.5f, 1.0f,   1.0f, 1.0f,  // Top-right (from back view)
    // Triangle 2
    -0.5f,  0.5f, -0.5f, 1.0f,   1.0f, 1.0f,  // Top-right (from back view)
     0.5f,  0.5f, -0.5f, 1.0f,   0.0f, 1.0f,  // Top-left (from back view)
     0.5f, -0.5f, -0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left (from back view)

    // Right face (X+)
    // Triangle 1
     0.5f, -0.5f,  0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f, -0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    // Triangle 2
     0.5f,  0.5f, -0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
     0.5f,  0.5f,  0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
     0.5f, -0.5f,  0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Left face (X-)
    // Triangle 1
    -0.5f, -0.5f, -0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
    -0.5f, -0.5f,  0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
    -0.5f,  0.5f,  0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    // Triangle 2
    -0.5f,  0.5f,  0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f, -0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.5f, -0.5f, -0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Top face (Y+)
    // Triangle 1
    -0.5f,  0.5f,  0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.5f,  0.5f,  0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.5f,  0.5f, -0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    // Triangle 2
     0.5f,  0.5f, -0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.5f,  0.5f, -0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.5f,  0.5f,  0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left

    // Bottom face (Y-)
    // Triangle 1
    -0.5f, -0.5f, -0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
     0.5f, -0.5f, -0.5f, 1.0f,   1.0f, 0.0f,  // Bottom-right
     0.5f, -0.5f,  0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    // Triangle 2
     0.5f, -0.5f,  0.5f, 1.0f,   1.0f, 1.0f,  // Top-right
    -0.5f, -0.5f,  0.5f, 1.0f,   0.0f, 1.0f,  // Top-left
    -0.5f, -0.5f, -0.5f, 1.0f,   0.0f, 0.0f,  // Bottom-left
});
// clang-format on

class BlockGameApplication : public Application {
    public:
	std::string LoadSource(const char *path)
	{
		std::ifstream file(path);
		LOG_CRITICAL_IF(Default, !file, "Failed to open file {}!",
				path);

		std::string str;

		file.seekg(0, std::ios::end);
		str.reserve(file.tellg());
		file.seekg(0, std::ios::beg);

		str.assign(std::istreambuf_iterator<char>(file),
			   std::istreambuf_iterator<char>());

		return str;
	}

	virtual void Init() override
	{
		std::string code = LoadSource("./assets/shader.wgsl");
		m_pipeline.Create(GetDevice(), code.c_str(),
				  GetSurfaceFormat());

		m_vertexBuffer = CreateBuffer(vertexData.data(),
					      vertexData.size() * sizeof(float),
					      wgpu::BufferUsage::Vertex);

		auto &window = GetWindow();

		m_uniformData.proj = glm::perspective(
			90.0f, (float)window.GetWidth() / window.GetHeight(),
			0.1f, 100.0f);

		m_uniformData.view = glm::mat4(1.0f);
		m_ssboData.model = glm::mat4(1.0f);

		m_uniformBuffer = CreateBuffer(&m_uniformData,
					       sizeof(m_uniformData),
					       wgpu::BufferUsage::Uniform);

		m_ssbo = CreateBuffer(&m_ssboData, sizeof(m_ssboData),
				      wgpu::BufferUsage::Storage);

		std::vector<wgpu::BindGroupEntry> entries = {
			wgpu::BindGroupEntry{
				.binding = 0,
				.buffer = m_uniformBuffer,
				.offset = 0,
				.size = sizeof(m_uniformData),
			},
			wgpu::BindGroupEntry{
				.binding = 1,
				.buffer = m_ssbo,
				.offset = 0,
				.size = sizeof(m_ssboData),
			}
		};

		wgpu::BindGroupDescriptor bindGroupDesc = {
			.layout = m_pipeline.GetBindGroupLayout(),
			.entryCount = entries.size(),
			.entries = entries.data(),
		};

		m_bindGroup = GetDevice().CreateBindGroup(&bindGroupDesc);

		for (int x = 0; x < WORLD_SIZE; x++) {
			for (int y = 0; y < WORLD_SIZE; y++) {
				for (int z = 0; z < WORLD_SIZE; z++) {
					world[x][y][z] = (y < WORLD_SIZE / 2);
				}
			}
		}
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

		wgpu::RenderPassDepthStencilAttachment depthStencilAttachment = {
			.view = m_pipeline.GetDepthStencilView(),
			.depthLoadOp = wgpu::LoadOp::Clear,
			.depthStoreOp = wgpu::StoreOp::Store,
			.depthClearValue = 1.0f,
			.depthReadOnly = false,
			.stencilLoadOp = wgpu::LoadOp::Undefined,
			.stencilStoreOp = wgpu::StoreOp::Undefined,
			.stencilClearValue = 0,
			.stencilReadOnly = true,
		};

		wgpu::RenderPassDescriptor renderPassDesc = {
			.colorAttachmentCount = 1,
			.colorAttachments = &attachment,
			.depthStencilAttachment = &depthStencilAttachment,
		};

		wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
		wgpu::RenderPassEncoder pass =
			encoder.BeginRenderPass(&renderPassDesc);

		pass.SetPipeline(m_pipeline.GetPipeline());
		pass.SetVertexBuffer(0, m_vertexBuffer);

		pass.SetBindGroup(0, m_bindGroup, 0, nullptr);

		pass.Draw(6 * 6);

		/*for (int x = 0; x < WORLD_SIZE; x++) {
			for (int y = 0; y < WORLD_SIZE; y++) {
				for (int z = 0; z < WORLD_SIZE; z++) {
					if (world[x][y][z]) {
						pass.Draw(6 * 6);
					}
				}
			}
		}*/

		pass.End();

		wgpu::CommandBuffer commands = encoder.Finish();
		device.GetQueue().Submit(1, &commands);
	}

	virtual void Update() override
	{
	}

	virtual void Destroy() override
	{
		m_bindGroup = nullptr;

		m_ssbo = nullptr;
		m_uniformBuffer = nullptr;
		m_vertexBuffer = nullptr;

		m_pipeline.Release();
	}

    private:
	RenderPipeline m_pipeline;

	wgpu::Buffer m_vertexBuffer;
	wgpu::Buffer m_uniformBuffer;
	wgpu::Buffer m_ssbo;

	wgpu::BindGroup m_bindGroup;
	UniformData m_uniformData;
	SSBOData m_ssboData;

	bool world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];

	glm::vec3 m_cameraPos;
	float m_yaw, m_pitch;
};

std::unique_ptr<Application> CreateApplication()
{
	Config &config = Config::Get();
	config.SetWidth(512);
	config.SetHeight(512);
	config.SetTitle("Block Game");

	return std::make_unique<BlockGameApplication>();
}
