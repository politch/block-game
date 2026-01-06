struct VertexInput {
  @location(0) position: vec4f,
  @location(1) uv: vec2f,
}

struct VertexOutput {
  @builtin(position) position: vec4f,

  @location(0) uv: vec2f,
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
  var out: VertexOutput;
  out.position = in.position;
  out.uv = in.uv;
  return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
  let color = vec3f(in.uv, 0.0);
  
  // Gamma Correction
  let linear_color = pow(color, vec3f(2.2));
  return vec4f(linear_color, 1.0);
}
