#version 460 core

#include "common.glsl"

in vec3 v_localPos;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_texMap;

const vec2 invAtan = vec2(0.1591f, 0.3183f);

vec2 sampleSphericalMap(vec3 v) {
  vec2 uv = vec2(atan(v.z, v.x + 1e-5f), asin(v.y));
  uv *= invAtan;
  uv += 0.5f;

  return uv;
}

void main() {
  vec2 uv = sampleSphericalMap(normalizeSafe(v_localPos));
  vec3 color = texture(u_texMap, uv).rgb;

  // if (isnan(color.r) || isinf(color.r) || isnan(color.g) || isinf(color.g) || isnan(color.b) || isinf(color.b))
  //   color = vec3(0.f);

  FragColor = vec4(color, 1.f);
}

