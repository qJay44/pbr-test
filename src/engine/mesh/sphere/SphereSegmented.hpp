#pragma once

#include "../MeshElements.hpp"
#include "../pbr/Model.hpp"

struct SphereSegmented : public pbr::Model {
  float radius;
  float heightScale = 1.f;
  MeshElements mesh;

  SphereSegmented() = default;
  SphereSegmented(size_t segments, float radius);

  void draw(const Camera* camera, Shader& shader) const;
};

