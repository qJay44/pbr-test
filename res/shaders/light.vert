#version 460 core

#include "common.glsl"

layout(location = 0) in vec3 a_pos;

out vec2 v_uv;

uniform mat4 u_camProj;
uniform mat4 u_camView;
uniform mat4 u_model;
uniform LightPoint u_light;

void main() {
  v_uv = a_pos.xz * 0.5f + 0.5f;

  vec3 worldPos = vec3(u_model * vec4(0.f, 0.f, 0.f, 1.f));
  vec4 viewPos = u_camView * vec4(worldPos, 1.f);

  viewPos.xy += a_pos.xz * u_light.radius;

  vec4 clipSpace = u_camProj * viewPos;

  // gl_Position = clipSpace.xyww; // Ignore camera far plane
  gl_Position = clipSpace;
}

