#pragma once

#include "../MeshElements.hpp"
#include "../Material.hpp"

class SphereSegmented {
public:
  SphereSegmented(size_t segments, float radius);

  void loadMaterial(fspath folderPath);
  void setHeightScale(float h);
  void draw(const Camera* camera, Shader& shader) const;

private:
  friend struct gui;

  float radius;
  float heightScale = 1.f;
  MeshElements mesh;

  Material material{};
};

