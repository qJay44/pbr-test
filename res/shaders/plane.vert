#version 460 core

layout (location = 0) in vec3 a_pos;

out VERTEX_DATA {
  vec3 normal;
  vec2 uv;
} data_out;

layout(binding = 3) uniform sampler2D u_texHeight;

void main() {
  data_out.normal = vec3(0.f, 1.f, 0.f);
  data_out.uv = a_pos.xz * 0.5f + 0.5f;

	gl_Position = vec4(a_pos, 1.f);
}

