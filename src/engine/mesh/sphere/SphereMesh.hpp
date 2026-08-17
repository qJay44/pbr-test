#pragma once

#include "../MeshElements.hpp"

class SphereMesh {
public:
  struct SphereFaceChunk {
    MeshElements mesh;

    SphereFaceChunk() = default;
    SphereFaceChunk(vec3 up, vec2 start, const SphereMesh* sm);

    static vec3 spherify(const vec3& v);
    static vec3 spherifyFancy(const vec3& v);
  };

  struct SphereFace {
    std::list<SphereFaceChunk> chunks;
    vec3 debugColor;

    void build(vec3 up, const SphereMesh* sm);
    void translate(vec3 v);
    void scale(float s);
    void draw(const Camera* camera, Shader& shader) const;
  };

  SphereMesh(int chunksPerSide, int resolution, float radius, GLenum mode = GL_TRIANGLES);

  void build();
  void translate(vec3 v);
  void scale(float s);
  void draw(const Camera* camera, Shader& shader) const;

private:
  int chunksPerSide;
  int resolution;
  float radius;
  GLenum renderPrimitive;

  SphereFace faces[6];
};

