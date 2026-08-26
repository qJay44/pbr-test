#include "SphereInflatedCube.hpp"

#include "../meshes.hpp"

SphereInflatedCube::SphereInflatedCube(size_t resolution, float radius, GLenum renderPrimitive)
  : radius(radius), mesh(meshes::plane(resolution, renderPrimitive))
{
  mesh.setInstanceCount(6);
}

void SphereInflatedCube::loadMaterial(fspath folderPath) {
  material.loadFrom(folderPath);
}

void SphereInflatedCube::draw(const Camera* camera, Shader& shader) const {
  material.setUniforms(shader);
  material.bindTextures();
  shader.setUniform1f("u_radius", radius);

  mesh.draw(camera, shader);
}

