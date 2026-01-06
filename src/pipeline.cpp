#include "pipeline.h"
#include "config.h"

#include "uniform.h"
#include "ssbo.h"

#include "webgpu/webgpu_cpp.h"
#include <vector>

void RenderPipeline::Create(wgpu::Device &device, const char *src,
			    wgpu::TextureFormat format)
{
	std::vector<wgpu::BindGroupLayoutEntry> entries = {
    wgpu::BindGroupLayoutEntry {
      .binding = 0,
      .visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
      .buffer = {
        .type = wgpu::BufferBindingType::Uniform,
        .minBindingSize = sizeof(UniformData),
      },
    },
    wgpu::BindGroupLayoutEntry {
      .binding = 1,
      .visibility = wgpu::ShaderStage::Vertex,
      .buffer = {
        .type = wgpu::BufferBindingType::ReadOnlyStorage,
        .minBindingSize = sizeof(SSBOData),
      },
    },
    wgpu::BindGroupLayoutEntry {
      .binding = 2,
      .visibility = wgpu::ShaderStage::Fragment,
      .texture = {
        .sampleType = wgpu::TextureSampleType::Float,
        .viewDimension = wgpu::TextureViewDimension::e2D,
      },
    }
  };

	wgpu::BindGroupLayoutDescriptor bindGroupDesc = {
		.entryCount = entries.size(),
		.entries = entries.data(),
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

	wgpu::TextureFormat depthStencilFormat =
		wgpu::TextureFormat::Depth24Plus;
	wgpu::DepthStencilState depthStencilState = {
		.format = depthStencilFormat,
		.depthWriteEnabled = true,
		.depthCompare = wgpu::CompareFunction::Less,
		.stencilReadMask = 0,
		.stencilWriteMask = 0,
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
    .depthStencil = &depthStencilState,
    .multisample = {
      .count = 1,
      .mask = ~0u,
      .alphaToCoverageEnabled = false,
    },
    .fragment = &fragmentState,
	};

	m_pipeline = device.CreateRenderPipeline(&desc);

	auto &config = Config::Get();
	int width = config.GetWidth();
	int height = config.GetHeight();

	wgpu::TextureDescriptor depthStencilDesc = {
    .usage = wgpu::TextureUsage::RenderAttachment,
    .dimension = wgpu::TextureDimension::e2D,
    .size = {
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
      .depthOrArrayLayers = 1,
    },
    .format = depthStencilFormat,
    .mipLevelCount = 1,
    .sampleCount = 1,
    .viewFormatCount = 1,
    .viewFormats = &depthStencilFormat,
  };

	m_depthStencil = device.CreateTexture(&depthStencilDesc);

	wgpu::TextureViewDescriptor depthStencilViewDesc = {
		.format = depthStencilFormat,
		.dimension = wgpu::TextureViewDimension::e2D,
		.baseMipLevel = 0,
		.mipLevelCount = 1,
		.baseArrayLayer = 0,
		.arrayLayerCount = 1,
		.aspect = wgpu::TextureAspect::DepthOnly,
	};

	m_depthStencilView = m_depthStencil.CreateView(&depthStencilViewDesc);
}

void RenderPipeline::Release()
{
	m_depthStencilView = nullptr;
	m_depthStencil = nullptr;
	m_pipeline = nullptr;
	m_layout = nullptr;
	m_bindGroupLayout = nullptr;
}
