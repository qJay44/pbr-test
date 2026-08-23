#version 460 core

#include "common.glsl"

in vec2 texCoord;

out vec4 FragColor;

uniform LightPoint u_light;

void main() {
  float dist = length(texCoord - 0.5f);
  float alpha = 1.f - smoothstep(0.f, 0.5f, dist);

  FragColor = vec4(u_light.color * u_light.multiplier, alpha);
}

