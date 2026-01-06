struct VertexInput {
  @location(0) position: vec4f,
  @location(1) uv: vec2f,
}

struct VertexOutput {
  @builtin(position) position: vec4f,

  @location(0) uv: vec2f,
}

struct UniformData {
  proj: mat4x4f,
  view: mat4x4f,
}

struct SSBOData {
  model: mat4x4f,
}

@group(0) @binding(0) var<uniform> uUniform: UniformData;
@group(0) @binding(1) var<storage, read> uSSBO: SSBOData;
@group(0) @binding(2) var texture: texture_2d<f32>;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var out: VertexOutput;
  out.position = uUniform.proj * uUniform.view * uSSBO.model * in.position;
  out.uv = in.uv;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  let texCoords = vec2i(in.uv * vec2f(textureDimensions(texture)));
  let color = textureLoad(texture, texCoords, 0).rgb;
  
  // Gamma Correction
  let linear_color = pow(color, vec3f(2.2));
  return vec4f(linear_color, 1.0);
}
