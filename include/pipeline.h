#pragma once

#include "webgpu.h"
#include "webgpu/webgpu_cpp.h"

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

    private:
	wgpu::BindGroupLayout m_bindGroupLayout;
	wgpu::PipelineLayout m_layout;
	wgpu::RenderPipeline m_pipeline;
};
