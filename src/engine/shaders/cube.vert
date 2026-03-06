#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTex;
layout (location = 3) in vec3 inNormal;

out vec4 worldPos;
out vec3 color;
out vec2 texCoord;
out vec3 normal;

uniform mat4 u_model;
uniform mat4 u_camPV;

void main() {
  worldPos = u_model * vec4(inPos, 1.f);
  color = inColor;
  texCoord = inTex;
  normal = inNormal;
	gl_Position = u_camPV * worldPos;
}

