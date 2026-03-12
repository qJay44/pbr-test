#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTex;

out vec3 v_worldPos;
out vec3 v_normal;

uniform mat4 u_model;
uniform mat4 u_camPV;

void main() {
  vec4 worldPos = u_model * vec4(inPos, 1.f);
  v_worldPos = worldPos.xyz;
  v_normal = inPos;
	gl_Position = u_camPV * worldPos;
}

