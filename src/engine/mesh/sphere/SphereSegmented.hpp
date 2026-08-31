#pragma once

#include "../MeshElements.hpp"
#include "../Material.hpp"

struct SphereSegmented {
  float radius;
  float heightScale = 1.f;
  MeshElements mesh;
  Material material{};

  SphereSegmented() = default;
  SphereSegmented(size_t segments, float radius);

  void draw(const Camera* camera, Shader& shader) const;
};

