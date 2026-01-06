#include "app.h"
#include "config.h"
#include "entrypoint.h"

#include <fstream>

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
		m_pipeline = CreateRenderPipeline(code.c_str());

		m_vertexBuffer = CreateBuffer(vertexData.data(),
					      vertexData.size() * sizeof(float),
					      wgpu::BufferUsage::Vertex);

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

		wgpu::RenderPassDescriptor renderPassDesc = {
			.colorAttachmentCount = 1,
			.colorAttachments = &attachment,
		};

		wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
		wgpu::RenderPassEncoder pass =
			encoder.BeginRenderPass(&renderPassDesc);

		pass.SetPipeline(m_pipeline);
		pass.SetVertexBuffer(0, m_vertexBuffer);

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
		if (m_vertexBuffer) {
			m_vertexBuffer = nullptr;
		}

		if (m_pipeline) {
			m_pipeline = nullptr;
		}
	}

    private:
	wgpu::RenderPipeline m_pipeline;
	wgpu::Buffer m_vertexBuffer;

	bool world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];
};

std::unique_ptr<Application> CreateApplication()
{
	Config &config = Config::Get();
	config.SetWidth(512);
	config.SetHeight(512);
	config.SetTitle("Block Game");

	return std::make_unique<BlockGameApplication>();
}
