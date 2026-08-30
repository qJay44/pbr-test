#version 460 core

#include "pbr.glsl"

in vec2 v_uv;

out vec4 FragColor;

void main() {
  float NdotV = v_uv.x;
  float roughness = v_uv.y;

  vec3 V;
  V.x = sqrt(1.f - sq(NdotV));
  V.y = 0.f;
  V.z = NdotV;

  float A = 0.f;
  float B = 0.f;
  vec3 N = vec3(0.f, 0.f, 1.f);

  const uint sampleCount = 1024;

  for (uint i = 0; i < sampleCount; i++) {
    vec2 Xi = Hammersley(i, sampleCount);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.f * dot(V, H) * H - V);

    float NdotL = max(L.z, 0.f);
    float NdotH = max(H.z, 0.f);
    float VdotH = dot0(V, H);

    if (NdotL > 0.f) {
      float G = GeometrySmith_IBL(roughness, N, V, L);
      float G_Vis = (G * VdotH) / (NdotH * NdotV);
      float Fc = pow(1.f - VdotH, 5.f);

      A += (1.f - Fc) * G_Vis;
      B += Fc * G_Vis;
    }
  }

  A /= float(sampleCount);
  B /= float(sampleCount);

  FragColor = vec4(A, B, 0.f, 1.f);
}

