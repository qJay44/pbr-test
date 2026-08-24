#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex;

out vec3 v_worldPos;
out vec3 v_normal;
out vec2 v_uv;

layout(binding = 3) uniform sampler2D u_texHeight;

uniform mat4 u_model;
uniform mat4 u_camPV;
uniform float u_time;

void main() {
  float h = texture(u_texHeight, a_tex + u_time * 0.01f).r;
  vec3 pos = a_pos;
  pos.y += h * 5.f;

  vec4 worldPos = u_model * vec4(pos, 1.f);

  v_worldPos = worldPos.xyz;
  v_normal = vec3(0.f, 1.f, 0.f);
  v_uv = a_tex;
	gl_Position = u_camPV * worldPos;
}

