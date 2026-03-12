#pragma once

#include "../Shader.hpp"

struct Material {
  vec3 albedo;
  vec3 emissivity;
  vec3 baseReflectivity;
  float metallic;
  float roughness;

  void setUniforms(Shader& shader) const {
    shader.setUniform3f("u_material.albedo", albedo);
    shader.setUniform3f("u_material.emissivity", emissivity);
    shader.setUniform3f("u_material.baseReflectivity", baseReflectivity);
    shader.setUniform1f("u_material.metallic", metallic);
    shader.setUniform1f("u_material.roughness", roughness);
  }
};

