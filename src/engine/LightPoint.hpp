#pragma once

#include "mesh/MeshElements.hpp"
#include "mesh/meshes.hpp"

class LightPoint {
public:
  LightPoint(vec3 position, vec3 color = {1.f, 1.f, 1.f}, vec3 multiplier = {1.f, 1.f, 1.f}, float radius = 10.f);

  const vec3& getPosition() const;
  const vec3& getColor() const;

  void update();
  void setUniforms(Shader& shader) const;
  void setUniformsNoId(Shader& shader) const;

  void draw(const Camera* camera, Shader& shader) const;

private:
  friend struct gui;

  static size_t nextId;

  MeshElements mesh = meshes::plane(2, GL_TRIANGLES);

  size_t id;
  vec3 position;
  vec3 color;
  vec3 multiplier{1.f}; // Lumens?
  float radius;
};

