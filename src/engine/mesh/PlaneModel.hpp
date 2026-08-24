#pragma once

#include "MeshElements.hpp"
#include "Material.hpp"

struct PlaneModel {
  MeshElements mesh;
  Material material{};

  void draw(const Camera* camera, Shader& shader) const {
    material.setUniforms(shader);
    material.bindTextures();

    mesh.draw(camera, shader);
  }
};

