#pragma once

#include "mesh/MeshElements.hpp"
#include "mesh/meshes.hpp"

#include "global.hpp"

class Light {
public:
  Light(vec3 position, vec3 color = {1.f, 1.f, 1.f}, vec3 multiplier = {1.f, 1.f, 1.f}, float radius = 30.f);

  const vec3& getPosition() const;
  const vec3& getColor() const;

  void update();
  void setUniforms(Shader& shader) const;

  void draw(const Camera* camera, Shader& shader) const;

private:
  friend struct gui;

  MeshElements mesh = meshes::plane(2, GL_TRIANGLES, global::forward);

  vec3 position;
  vec3 color;
  vec3 multiplier{1.f}; // Lumens?
  float radius;
};

