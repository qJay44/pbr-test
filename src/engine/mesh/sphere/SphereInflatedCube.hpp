#pragma once

#include "../MeshElementsInstancing.hpp"
#include "../pbr/Model.hpp"

struct SphereInflatedCube : public pbr::Model {
  float radius;
  MeshElementsInstancing mesh;

  SphereInflatedCube(size_t resolution, float radius, GLenum renderPrimitive = GL_TRIANGLES);

  void draw(const Camera* camera, Shader& shader) const;
};

