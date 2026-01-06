#include "app.h"
#include "config.h"
#include "entrypoint.h"

#include "pipeline.h"
#include "ssbo.h"
#include "texture.h"
#include "uniform.h"

#include <algorithm>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <stb_image.h>

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

	int GetBlockIdx(int x, int y, int z)
	{
		return z + WORLD_SIZE * (y + WORLD_SIZE * x);
	}

	uint32_t GetSSBOElementSize()
	{
		uint32_t minSSBOStride = GetMinSSBOStride();
		return (sizeof(SSBOData) + minSSBOStride - 1) / minSSBOStride *
		       minSSBOStride;
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

		m_uniformBuffer = CreateBuffer(&m_uniformData,
					       sizeof(m_uniformData),
					       wgpu::BufferUsage::Uniform);

		uint32_t ssboElementSize = GetSSBOElementSize();

		m_ssbo = CreateBuffer(nullptr,
				      ssboElementSize * WORLD_SIZE *
					      WORLD_SIZE * WORLD_SIZE,
				      wgpu::BufferUsage::Storage);

		for (int x = 0; x < WORLD_SIZE; x++) {
			for (int y = 0; y < WORLD_SIZE; y++) {
				for (int z = 0; z < WORLD_SIZE; z++) {
					int idx = GetBlockIdx(x, y, z);

					SSBOData data;

					data.model = glm::translate(
						glm::mat4(1.0f),
						glm::vec3(x, y, z) -
							glm::vec3(WORLD_SIZE /
									  2.0f,
								  4,
								  WORLD_SIZE /
									  2.0f));

					GetDevice().GetQueue().WriteBuffer(
						m_ssbo, ssboElementSize * idx,
						&data, sizeof(data));
				}
			}
		}

		stbi_set_flip_vertically_on_load(true);

		int image_width, image_height, image_channels;
		unsigned char *image = stbi_load("./assets/cobblestone.png",
						 &image_width, &image_height,
						 &image_channels, 0);

		m_texture.Create(GetDevice(), image, image_width, image_height);

		stbi_image_free(image);

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
				.size = sizeof(SSBOData),
			},
			wgpu::BindGroupEntry{
				.binding = 2,
				.textureView = m_texture.GetView(),
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
					world[x][y][z] = (y < 3);
				}
			}
		}
	}

	virtual void Render() override
	{
		wgpu::Device &device = GetDevice();
		wgpu::Surface &surface = GetSurface();

		device.GetQueue().WriteBuffer(m_uniformBuffer, 0,
					      &m_uniformData,
					      sizeof(m_uniformData));

		wgpu::SurfaceTexture surfaceTexture;
		surface.GetCurrentTexture(&surfaceTexture);

		wgpu::RenderPassColorAttachment attachment = {
			.view = surfaceTexture.texture.CreateView(),
			.loadOp = wgpu::LoadOp::Clear,
			.storeOp = wgpu::StoreOp::Store,
      .clearValue = {
        .r = 110 / 255.0f,
        .g = 117 / 255.0f,
        .b = 255 / 255.0f,
        .a = 1.0f,
      },
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

		for (int x = 0; x < WORLD_SIZE; x++) {
			for (int y = 0; y < WORLD_SIZE; y++) {
				for (int z = 0; z < WORLD_SIZE; z++) {
					if (world[x][y][z]) {
						uint32_t offset =
							GetBlockIdx(x, y, z) *
							GetSSBOElementSize();

						pass.SetBindGroup(0,
								  m_bindGroup,
								  1, &offset);

						pass.Draw(6 * 6);
					}
				}
			}
		}

		pass.End();

		wgpu::CommandBuffer commands = encoder.Finish();
		device.GetQueue().Submit(1, &commands);
	}

	virtual void Update(float deltaTime) override
	{
		auto &window = GetWindow();

		auto delta = window.GetCursorDelta();

		m_yaw -= delta.x * m_sensitivity * deltaTime;
		m_yaw = std::fmod(m_yaw, 360.0f);

		m_pitch = std::clamp(m_pitch - delta.y * m_sensitivity *
						       deltaTime,
				     -89.0f, 89.0f);

		glm::quat yawRotation = glm::angleAxis(
			glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::quat pitchRotation = glm::angleAxis(
			glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));

		glm::quat rotation = yawRotation * pitchRotation;

		glm::vec3 movement(0.0f);

		if (window.IsKeyPressed(GLFW_KEY_W)) {
			glm::vec3 dir = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
			dir.y = 0;
			dir = glm::normalize(dir);

			movement += dir;
		}

		if (window.IsKeyPressed(GLFW_KEY_S)) {
			glm::vec3 dir = rotation * glm::vec3(0.0f, 0.0f, 1.0f);
			dir.y = 0;
			dir = glm::normalize(dir);

			movement += dir;
		}

		if (window.IsKeyPressed(GLFW_KEY_A)) {
			glm::vec3 dir = rotation * glm::vec3(-1.0f, 0.0f, 0.0f);
			dir.y = 0;
			dir = glm::normalize(dir);

			movement += dir;
		}

		if (window.IsKeyPressed(GLFW_KEY_D)) {
			glm::vec3 dir = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
			dir.y = 0;
			dir = glm::normalize(dir);

			movement += dir;
		}

		if (window.IsKeyPressed(GLFW_KEY_SPACE)) {
			movement += glm::vec3(0.0f, 1.0f, 0.0f);
		}

		if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			movement += glm::vec3(0.0f, -1.0f, 0.0f);
		}

		if (glm::length(movement) > 0.0f) {
			movement = glm::normalize(movement);
		}

		m_cameraPos += movement * m_speed * deltaTime;

		glm::mat4 translationMatrix =
			glm::translate(glm::mat4(1.0f), m_cameraPos);

		glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

		glm::mat4 cameraMatrix = translationMatrix * rotationMatrix;

		m_uniformData.view = glm::inverse(cameraMatrix);
	}

	virtual void Destroy() override
	{
		m_bindGroup = nullptr;

		m_texture.Release();

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

	Texture m_texture;

	wgpu::BindGroup m_bindGroup;
	UniformData m_uniformData;

	bool world[WORLD_SIZE][WORLD_SIZE][WORLD_SIZE];

	glm::vec3 m_cameraPos = { 0.0f, 0.0f, 0.0f };
	float m_yaw, m_pitch;

	const float m_sensitivity = 20.0f;
	const float m_speed = 5.0f;
};

std::unique_ptr<Application> CreateApplication()
{
	Config &config = Config::Get();
	config.SetWidth(512);
	config.SetHeight(512);
	config.SetTitle("Block Game");

	return std::make_unique<BlockGameApplication>();
}
