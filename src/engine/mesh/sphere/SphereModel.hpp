#pragma once

#include "../MeshElementsInstancing.hpp"
#include "../Material.hpp"

class SphereModel {
public:
  SphereModel(size_t resolution, float radius, GLenum renderPrimitive = GL_TRIANGLES);

  void loadMaterial(fspath folderPath);
  void draw(const Camera* camera, Shader& shader) const;

private:
  friend struct gui;

  float radius;
  MeshElementsInstancing mesh;

  Material material{};
};

