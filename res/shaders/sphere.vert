#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex;

out vec3 v_worldPos;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_camPV;

void main() {
  vec4 worldPos = u_model * vec4(a_pos, 1.f);
  v_worldPos = worldPos.xyz;
  v_normal = a_pos;
	gl_Position = u_camPV * worldPos;
}

