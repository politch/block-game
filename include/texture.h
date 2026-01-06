#pragma once

#include "webgpu.h"

class Texture {
    public:
	Texture() = default;
	~Texture() = default;

	void Create(wgpu::Device &device, void *data, int width, int height);
	void Release();

	inline wgpu::Texture &GetTexture()
	{
		return m_texture;
	}

	inline wgpu::TextureView &GetView()
	{
		return m_view;
	}

    private:
	wgpu::Texture m_texture;
	wgpu::TextureView m_view;
};
