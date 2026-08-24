#include "SphereModel.hpp"

#include "../meshes.hpp"

SphereModel::SphereModel(size_t resolution, float radius, GLenum renderPrimitive)
  : radius(radius), mesh(meshes::plane(resolution, renderPrimitive))
{
  mesh.setInstanceCount(6);
}

void SphereModel::loadMaterial(fspath folderPath) {
  material.loadFrom(folderPath);
}

void SphereModel::draw(const Camera* camera, Shader& shader) const {
  material.setUniforms(shader);
  material.bindTextures();
  shader.setUniform1f("u_radius", radius);

  mesh.draw(camera, shader);
}

