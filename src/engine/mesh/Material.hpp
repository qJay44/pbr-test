#pragma once

#include "../Shader.hpp"
#include "f0.hpp"

struct Material {
  vec3 albedo;
  vec3 emissivity;
  float metallic;
  float roughness;
  f0::index baseReflectivityIdx;

  void setUniforms(Shader& shader) const {
    shader.setUniform3f("u_material.albedo", albedo);
    shader.setUniform3f("u_material.emissivity", emissivity);
    shader.setUniform3f("u_material.baseReflectivity", f0::all[baseReflectivityIdx].color);
    shader.setUniform1f("u_material.metallic", metallic);
    shader.setUniform1f("u_material.roughness", roughness);
  }
};

