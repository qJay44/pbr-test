#include "SphereSegmented.hpp"

#include "../../environment.hpp"

SphereSegmented::SphereSegmented(size_t segments, float radius) : radius(radius) {
  std::vector<vertex::PTN> vertices;
  std::vector<GLuint> indices;
  float invSegments = 1.f / segments;

  for (size_t x = 0; x <= segments; ++x) {
    for (size_t y = 0; y <= segments; ++y) {
      float xSegment = x * invSegments;
      float ySegment = y * invSegments;
      float xPos = cos(xSegment * 2.f * PI) * sin(ySegment * PI);
      float yPos = cos(ySegment * PI);
      float zPos = sin(xSegment * 2.f * PI) * sin(ySegment * PI);

      vertex::PTN vertex;
      vertex.position = vec3(xPos, yPos, zPos);
      vertex.texture = vec2(xSegment, ySegment);
      vertex.normal = vec3(xPos, yPos, zPos);

      vertices.push_back(vertex);
    }
  }

  bool oddRow = false;
  for (unsigned int y = 0; y < segments; ++y) {
    if (oddRow) {
      for (int x = segments; x >= 0; --x) {
        indices.push_back((y + 1) * (segments + 1) + x);
        indices.push_back(y * (segments + 1) + x);
      }
    } else {
      for (unsigned int x = 0; x <= segments; ++x) {
        indices.push_back(y * (segments + 1) + x);
        indices.push_back((y + 1) * (segments + 1) + x);
      }
    }
    oddRow = !oddRow;
  }

  MeshData data(vertices, indices);
  data.mode = GL_TRIANGLE_STRIP;

  mesh = MeshElements(data);
}

void SphereSegmented::draw(const Camera* camera, Shader& shader) const {
  material.setUniforms(shader);
  material.bindTextures();

  shader.setUniform1f("u_radius", radius);
  shader.setUniform1f("u_heightScale", heightScale);
  shader.setUniform1f("u_maxReflectionLod", environment::maxMipLevels);

  environment::texBrdfLut.bind(7);
  environment::texEnvPrefilterCubemapHDR.bind(8);
  environment::texIrradianceCubemapHDR.bind(9);

  mesh.draw(camera, shader);
}

