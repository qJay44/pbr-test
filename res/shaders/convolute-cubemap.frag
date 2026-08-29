#version 460 core

#include "common.glsl"

in vec3 v_localPos;

out vec4 FragColor;

layout(binding = 0) uniform samplerCube u_texEnvCubemapHDR;

void main() {
  vec3 irradiance = vec3(0.f);
  vec3 normal = normalizeSafe(v_localPos);
  vec3 up = vec3(0.f, 1.f, 0.f);
  vec3 right = normalize(cross(up, normal));
  up = normalize(cross(normal, right));

  float sampleDelta = TAU / 1024.f;
  float nrSamples = 0.f;
  for (float phi = 0.f; phi < TAU; phi += sampleDelta) {
    for (float theta = 0.f; theta < PI_2; theta += sampleDelta) {
      vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
      vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

      irradiance += texture(u_texEnvCubemapHDR, sampleVec).rgb * cos(theta) * sin(theta);
      nrSamples++;
    }
  }

  irradiance = PI * irradiance * (1.f / nrSamples);

  FragColor = vec4(irradiance, 1.f);
}

