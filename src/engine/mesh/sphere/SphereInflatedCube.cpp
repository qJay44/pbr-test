#include "SphereInflatedCube.hpp"

#include "../meshes.hpp"

SphereInflatedCube::SphereInflatedCube(size_t resolution, float radius, GLenum renderPrimitive)
  : radius(radius), mesh(meshes::plane(resolution, renderPrimitive))
{
  mesh.setInstanceCount(6);
}

void SphereInflatedCube::draw(const Camera* camera, Shader& shader) const {
  shader.setUniform1f("u_radius", radius);
  preDraw(shader);

  mesh.draw(camera, shader);
}

