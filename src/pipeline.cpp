#include "pipeline.h"
#include "webgpu/webgpu_cpp.h"

#include "uniform.h"

void RenderPipeline::Create(wgpu::Device &device, const char *src,
			    wgpu::TextureFormat format)
{
	wgpu::BindGroupLayoutEntry bindingLayout = {
		.binding = 0,
		.visibility = wgpu::ShaderStage::Vertex |
			      wgpu::ShaderStage::Fragment,
    .buffer = {
      .type = wgpu::BufferBindingType::Uniform,
      .minBindingSize = sizeof(UniformData),
    },
	};

	wgpu::BindGroupLayoutDescriptor bindGroupDesc = {
		.entryCount = 1,
		.entries = &bindingLayout,
	};

	m_bindGroupLayout = device.CreateBindGroupLayout(&bindGroupDesc);

	wgpu::PipelineLayoutDescriptor pipelineLayoutDesc = {
		.bindGroupLayoutCount = 1,
		.bindGroupLayouts = &m_bindGroupLayout,
	};

	m_layout = device.CreatePipelineLayout(&pipelineLayoutDesc);

	wgpu::ShaderSourceWGSL wgsl({
		.code = src,
	});

	wgpu::ShaderModuleDescriptor shaderModuleDesc = {
		.nextInChain = &wgsl,
	};

	wgpu::ShaderModule module =
		device.CreateShaderModule(&shaderModuleDesc);

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
		.format = format,
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

	std::vector<wgpu::VertexAttribute> attributes = {
		wgpu::VertexAttribute{
			.format = wgpu::VertexFormat::Float32x4,
			.offset = 0,
			.shaderLocation = 0,
		},
		wgpu::VertexAttribute{
			.format = wgpu::VertexFormat::Float32x2,
			.offset = 4 * sizeof(float),
			.shaderLocation = 1,
		}
	};

	wgpu::VertexBufferLayout vertexBufferLayout = {
		.stepMode = wgpu::VertexStepMode::Vertex,
		.arrayStride = 6 * sizeof(float),
		.attributeCount = attributes.size(),
		.attributes = attributes.data(),
	};

	wgpu::RenderPipelineDescriptor desc = {
    .layout = m_layout,
		.vertex = {
      .module = module,
      .entryPoint = "vs_main",
      .constantCount = 0,
      .constants = nullptr,
      .bufferCount = 1,
      .buffers = &vertexBufferLayout,
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

	m_pipeline = device.CreateRenderPipeline(&desc);
}

void RenderPipeline::Release()
{
	m_pipeline = nullptr;
	m_layout = nullptr;
	m_bindGroupLayout = nullptr;
}
