#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex;
layout (location = 2) in vec3 a_normal;

out VERTEX_DATA {
  vec3 normal;
  vec2 uv;
} data_out;

uniform float u_radius;

void main() {
  data_out.normal = a_normal;
  data_out.uv = a_tex;

	gl_Position = vec4(a_pos * u_radius, 1.f);
}

