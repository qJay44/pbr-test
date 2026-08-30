#version 460 core

#include "../pbr.glsl"

in vec3 v_localPos;

out vec4 FragColor;

layout(binding = 0) uniform samplerCube u_texEnvCubemapHDR;

uniform float u_roughness;

void main() {
  vec3 N = normalize(v_localPos);
  vec3 R = N;
  vec3 V = R;

  const uint sampleCount = 1024;
  float totalWeight = 0.f;
  vec3 color = vec3(0.f);

  for (uint i = 0; i < sampleCount; i++) {
    vec2 Xi = Hammersley(i, sampleCount);
    vec3 H = ImportanceSampleGGX(Xi, N, u_roughness);
    vec3 L = normalize(2.f * dot(V, H) * H - V);

    float NdotL = dot0(N, L);
    if (NdotL > 0.f) {
      float D = DistributionGGX(u_roughness, N, H);
      float pdf = (D * dot0(N, H) / (4.f * dot0(H, V))) + 1e-4f;

      float resolution = textureSize(u_texEnvCubemapHDR, 0).x;
      float saTexel = 4.f * PI / (6.f * sq(resolution));
      float saSample = 1.f / (float(sampleCount) * pdf + 1e-4f);

      float mipLevel = u_roughness == 0.f ? 0.f : 0.5f * log2(saSample / saTexel);

      color += textureLod(u_texEnvCubemapHDR, L, mipLevel).rgb * NdotL;
      totalWeight += NdotL;
    }
  }

  color /= totalWeight;

  FragColor = vec4(color, 1.f);
}

