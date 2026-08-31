#version 460 core

#include "pbr.glsl"

in vec3 v_worldPos;
in vec3 v_normal;
in vec2 v_uv;
in mat3 v_tbn;

out vec4 FragColor;

// From material
layout(binding = 0) uniform sampler2D u_texAlbedo;
layout(binding = 1) uniform sampler2D u_texAmbientOcclusion;
layout(binding = 2) uniform sampler2D u_texEmissive;
layout(binding = 3) uniform sampler2D u_texHeight;
layout(binding = 4) uniform sampler2D u_texMetallic;
layout(binding = 5) uniform sampler2D u_texNormal;
layout(binding = 6) uniform sampler2D u_texRoughness;

// From environment
layout(binding = 7) uniform sampler2D u_texBrdfLut;
layout(binding = 8) uniform samplerCube u_texEnvPrefilterCubemapHDR;
layout(binding = 9) uniform samplerCube u_texIrradianceCubemapHDR;

uniform Material u_customMaterial;
uniform LightPoint u_light0;
uniform LightPoint u_light1;
uniform LightPoint u_light2;
uniform LightPoint u_light3;
uniform vec3 u_camPos;
uniform vec3 u_baseReflectivity;
uniform float u_maxReflectionLod;
uniform bool u_useCustomMaterial;

LightPoint lights[4] = { u_light0, u_light1, u_light2, u_light3 }; // HACK: uuuughh

vec3 getNormalFromTangentMap(sampler2D normalMap, vec2 uv) {
  vec3 tangentNormal = texture(normalMap, uv).rgb * 2.f - 1.f;

  return normalize(v_tbn * tangentNormal);
}

vec3 getNormalFromTangentMapFrag(sampler2D normalMap, vec2 uv) {
  vec3 tangentNormal = texture(normalMap, uv).rgb * 2.f - 1.f;

  vec3 q1 = dFdx(v_worldPos);
  vec3 q2 = dFdy(v_worldPos);
  vec2 st1 = dFdx(uv);
  vec2 st2 = dFdy(uv);

  vec3 N = normalize(v_normal);
  vec3 T = normalize(q1 * st2.t - q2 * st1.t);
  vec3 B = -normalize(cross(N, T));
  mat3 TBN = mat3(T, B, N);

  return normalize(TBN * tangentNormal);
}

Material getMaterial(vec2 uv) {
  Material material;
  material.baseReflectivity = u_baseReflectivity;

  if (u_useCustomMaterial) {
    material = u_customMaterial;
    material.normal = normalize(v_normal);
  } else {
    material.albedo     = texture(u_texAlbedo, uv).rgb;
    material.emissivity = texture(u_texEmissive, uv).rgb;
    material.normal     = getNormalFromTangentMapFrag(u_texNormal, uv);
    material.metallic   = texture(u_texMetallic, uv).r;
    material.roughness  = texture(u_texRoughness, uv).r;
    material.ao         = texture(u_texAmbientOcclusion, uv).r;
  }

  return material;
}

vec3 getIrradianceAmbient(Material material, vec3 V) {
  vec3 N = material.normal;
  vec3 R = reflect(-V, N);
  vec3 f0 = mix(material.baseReflectivity, material.albedo, material.metallic);
  vec3 F = FresnelSchlickRoughness(f0, dot0(N, V), material.roughness);

  vec3 Ks = F;
  vec3 Kd = (1.f - Ks) * (1.f - material.metallic);
  vec3 irradiance = texture(u_texIrradianceCubemapHDR, N).rgb;
  vec3 diffuse = irradiance * material.albedo;

  vec3 prefilterColor = textureLod(u_texEnvPrefilterCubemapHDR, R, material.roughness * u_maxReflectionLod).rgb;
  vec2 envBRDF = texture(u_texBrdfLut, vec2(dot0(N, V), material.roughness)).rg;
  vec3 specular = prefilterColor * (F * envBRDF.x + envBRDF.y);

  vec3 ambient = (Kd * diffuse + specular) * material.ao;

  return ambient;
}

void main() {
  vec3 V = normalize(u_camPos - v_worldPos);    // View vector (omega 0)

  Material material = getMaterial(v_uv);
  vec3 Lo = vec3(0.f);
  float dw = 1.f / 4;

  for (int i = 0; i < 4; i++) {
    LightPoint light = lights[i];

    vec3 L = normalize(light.pos - v_worldPos); // Towards light direction (omega i)
    vec3 H = normalize(V + L);                  // Halfway vector
    float geomSmith = GeometrySmith_Direct(material.roughness, material.normal, V, L);

    float lightDist = distance(light.pos, v_worldPos);
    float attenuation = 1.f / sq(lightDist);
    vec3 radiance = light.color * light.multiplier * attenuation;

    vec3 brdf = BRDF_CookTorrance(material, V, L, H, geomSmith);
    Lo += brdf * radiance * dot0(material.normal, L);
  }

  vec3 ambient = getIrradianceAmbient(material, V);
  vec3 color = Lo + ambient + material.emissivity;
  color /= color + vec3(1.f);
  color = pow(color, vec3(1.f / 2.2f));

	FragColor = vec4(color, 1.f);
}

