#version 460 core

#define PI 3.141592265359f

in vec3 v_worldPos;
in vec3 v_normal;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_debug0Tex;

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
float D(float alpha, vec3 N, vec3 H) {
  float numerator = pow(alpha, 2.f);
  float NdotH = max(dot(N, H), 0.f);
  float denominator = PI * pow(pow(NdotH, 2.f) * (pow(alpha, 2.f) - 1.f) + 1.f, 2.f);
  denominator = max(denominator, epsilon);

  return numerator / denominator;
}

// Schlick-Beckmann Geometry Shadowing Function
float G1(float alpha, vec3 N, vec3 X) {
  float numerator = max(dot(N, X), 0.f);
  float k = alpha * 0.5f;
  float denominator = max(dot(N, X), 0.f) * (1.f - k) + k;
  denominator = max(denominator, epsilon);

  return numerator / denominator;
}

// Smith Model
float G(float alpha, vec3 N, vec3 V, vec3 L) {
  return G1(alpha, N, V) * G1(alpha, N, L);
}

// Fresnel-Schlick Function
vec3 F(vec3 f0, vec3 V, vec3 H) {
  return f0 + (vec3(1.f) - f0) * pow(1.f - max(dot(V, H), 0.f), 5.f);
}

void main() {
  vec3 N = normalize(v_normal);                // Mesh normal
  vec3 V = normalize(u_camPos - v_worldPos);   // View vector
  vec3 L = normalize(u_lightPos - v_worldPos); // Towards light direction
  vec3 H = normalize(V + L);                   // Halfway vector

  vec3 f0 = u_material.baseReflectivity;
  float alpha = u_material.roughness * u_material.roughness;

  vec3 Ks = F(f0, V, H);
  vec3 Kd = (vec3(1.f) - Ks) * (1.f - u_material.metallic);
  vec3 lambert = u_material.albedo / PI;

  vec3 cookTorranceNumerator = D(alpha, N, H) * G(alpha, N, V, L) * F(f0, V, H);
  float cookTorranceDenominator = 4.f * max(dot(V, N), 0.f) * max(dot(L, N), 0.f);
  cookTorranceDenominator = max(cookTorranceDenominator, epsilon);
  vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

  vec3 brdf = Kd * lambert + cookTorrance;
  vec3 col = u_material.emissivity + brdf * u_lightColor * max(dot(L, N), 0.f);

	FragColor = vec4(col, 1.f);
}

