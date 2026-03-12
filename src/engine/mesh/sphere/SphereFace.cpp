#include "SphereMesh.hpp"

using SphereFace = SphereMesh::SphereFace;

void SphereFace::build(vec3 up, const SphereMesh* sm) {
  chunks.clear();
  debugColor = max(up, abs(up) * 0.5f);

  for (int y = 0; y < sm->chunksPerSide; y++)
    for (int x = 0; x < sm->chunksPerSide; x++)
      chunks.push_back(SphereFaceChunk(up, {x, y}, sm));
}

void SphereFace::translate(vec3 v) {
  for (SphereFaceChunk& chunk : chunks)
    chunk.translate(v);
}

void SphereFace::scale(float s) {
  for (SphereFaceChunk& chunk : chunks)
    chunk.scale(s);
}

void SphereFace::draw(const Camera* camera, Shader& shader) const {
  for (const SphereFaceChunk& chunk : chunks)
    chunk.draw(camera, shader);
}

