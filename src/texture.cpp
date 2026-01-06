#include "texture.h"

#include "webgpu/webgpu_cpp.h"
#include <glm/glm.hpp>

void Texture::Create(wgpu::Device &device, void *data, int width, int height)
{
	Release();

	wgpu::TextureDescriptor textureDesc = {
    .usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding,
    .dimension = wgpu::TextureDimension::e2D,
    .size = {
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
      .depthOrArrayLayers = 1,
    },
    .format = wgpu::TextureFormat::RGBA8Unorm,
    .mipLevelCount = 1,
    .sampleCount = 1,
    .viewFormatCount = 0,
    .viewFormats = nullptr,
  };

	m_texture = device.CreateTexture(&textureDesc);

	wgpu::TexelCopyTextureInfo destination = {
    .texture = m_texture,
    .mipLevel = 0,
    .origin = {
      .x = 0,
      .y = 0,
      .z = 0,
    },
    .aspect = wgpu::TextureAspect::All,
	};

	wgpu::TexelCopyBufferLayout source = {
		.offset = 0,
		.bytesPerRow = 4 * textureDesc.size.width,
		.rowsPerImage = textureDesc.size.height,
	};

	device.GetQueue().WriteTexture(&destination, data,
				       width * height * sizeof(glm::vec4),
				       &source, &textureDesc.size);

	wgpu::TextureViewDescriptor viewDesc = {
		.format = textureDesc.format,
		.dimension = wgpu::TextureViewDimension::e2D,
		.baseMipLevel = 0,
		.mipLevelCount = 1,
		.baseArrayLayer = 0,
		.arrayLayerCount = 1,
		.aspect = wgpu::TextureAspect::All,
	};

	m_view = m_texture.CreateView(&viewDesc);
}

void Texture::Release()
{
	m_view = nullptr;
	m_texture = nullptr;
}
