#pragma once

#include "webgpu.h"

class RenderPipeline {
    public:
	RenderPipeline() = default;
	~RenderPipeline() = default;

	void Create(wgpu::Device &device, const char *src,
		    wgpu::TextureFormat format);

	void Release();

	inline wgpu::BindGroupLayout &GetBindGroupLayout()
	{
		return m_bindGroupLayout;
	}

	inline wgpu::PipelineLayout &GetLayout()
	{
		return m_layout;
	}

	inline wgpu::RenderPipeline &GetPipeline()
	{
		return m_pipeline;
	}

	inline wgpu::Texture GetDepthStencil()
	{
		return m_depthStencil;
	}

	inline wgpu::TextureView GetDepthStencilView()
	{
		return m_depthStencilView;
	}

    private:
	wgpu::BindGroupLayout m_bindGroupLayout;
	wgpu::PipelineLayout m_layout;
	wgpu::RenderPipeline m_pipeline;
	wgpu::Texture m_depthStencil;
	wgpu::TextureView m_depthStencilView;
};
