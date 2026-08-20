#version 460 core

#include "common.glsl"

in vec3 v_worldPos;
in vec3 v_normal;

out vec4 FragColor;

struct Material {
  vec3 albedo;
  vec3 emissivity;
  vec3 baseReflectivity;
  float metallic;
  float roughness;
};

uniform Material u_material;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_camPos;
uniform float u_time;

const float epsilon = 1e-6f;

// GGX/Trowbridge-Reitz Normal Distribution Function
float DistributionGGX(float alpha, vec3 N, vec3 H) {
  float a2 = sq(alpha);
  float NdotH = dot0(N, H);
  float NdotH2 = sq(NdotH);

  float numerator = a2;
  float denominator = PI * sq(NdotH2 * (a2 - 1.f) + 1.f);

  return numerator / max(denominator, epsilon);
}

// Schlick-Beckmann Geometry Shadowing Function
float GeometrySchlickGGX(float NdotX, float k) {
  float numerator = NdotX;
  float denominator = NdotX * (1.f - k) + k;

  return numerator / max(denominator, epsilon);
}

// Smith Model
float GeometrySmith(float alpha, vec3 N, vec3 V, vec3 L) {
  float NdotV = dot0(N, V);
  float NdotL = dot0(N, L);
  float k = sq(alpha + 1.f) * 0.125f; // Direct
  // float k = sq(alpha) * 0.5f; // IBL

  return GeometrySchlickGGX(NdotV, k) * GeometrySchlickGGX(NdotL, k);
}

// Fresnel-Schlick Function
vec3 FresnelSchlick(vec3 f0, float cosTheta) {
  return f0 + (vec3(1.f) - f0) * pow(1.f - cosTheta, 5.f);
}

vec3 BRDF_CookTorrance(float alpha, vec3 N, vec3 V, vec3 L, vec3 H) {
  vec3 lambert = u_material.albedo / PI;
  vec3 f0 = mix(u_material.baseReflectivity, u_material.albedo, u_material.metallic);

  float D = DistributionGGX(alpha, N, H);
  float G = GeometrySmith(alpha, N, V, L);
  vec3 F = FresnelSchlick(f0, dot0(N, H));

  vec3 numerator = D * F * G;
  float denominator = 4.f * dot0(N, V) * dot0(N, L);
  vec3 Ks = numerator / max(denominator, epsilon);
  vec3 Kd = (vec3(1.f) - Ks) * (1.f - u_material.metallic);

  return Kd * lambert + Ks;
}

void main() {
  vec3 N = normalize(v_normal);                // Mesh normal
  vec3 V = normalize(u_camPos - v_worldPos);   // View vector (omega 0)
  vec3 L = normalize(u_lightPos - v_worldPos); // Towards light direction (omega i)
  vec3 H = normalize(V + L);                   // Halfway vector

  vec3 brdf = BRDF_CookTorrance(u_material.roughness, N, V, L, H);
  vec3 col = u_material.emissivity + brdf * u_lightColor * dot0(N, L);

  col = pow(col, vec3(1.f / 2.2f));

	FragColor = vec4(col, 1.f);
}

