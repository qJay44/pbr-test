#include "SphereMesh.hpp"

SphereMesh::SphereMesh(int chunksPerSide, int resolution, float radius, GLenum renderPrimitive)
  : chunksPerSide(chunksPerSide), resolution(resolution), radius(radius), renderPrimitive(renderPrimitive)
{
  build();
}

void SphereMesh::build() {
  constexpr vec3 directions[6] {
    {1.f,  0.f,  0.f},  // Right
    {-1.f, 0.f,  0.f},  // Left
    {0.f,  1.f,  0.f},  // Top
    {0.f,  -1.f, 0.f},  // Bottom
    {0.f,  0.f,  1.f},  // Back
    {0.f,  0.f,  -1.f}, // Front
  };

  for (int i = 0; i < 6; i++)
    faces[i].build(directions[i], this);
}

void SphereMesh::translate(vec3 v) {
  for (SphereFace& face : faces)
    face.translate(v);
}

void SphereMesh::scale(float s) {
  for (SphereFace& face : faces)
    face.scale(s);
}

void SphereMesh::draw(const Camera* camera, Shader& shader) const {
  for (const SphereFace& face : faces)
    face.draw(camera, shader);
}

