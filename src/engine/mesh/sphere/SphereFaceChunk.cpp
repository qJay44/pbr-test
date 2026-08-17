#include "SphereMesh.hpp"

using SphereFaceChunk = SphereMesh::SphereFaceChunk;

SphereFaceChunk::SphereFaceChunk(vec3 up, vec2 start, const SphereMesh* sm) {
  int res = sm->resolution;

  std::vector<vertex::PT> vertices;
  std::vector<GLuint> indices;
  size_t triIndex = 0;

  const auto appendIndices_TRIANGLES = [&] (size_t idx) {
    indices[triIndex + 0] = idx + res + 1;  // 0       2 -------- 1
    indices[triIndex + 1] = idx + 1;        // 1       |          |
    indices[triIndex + 2] = idx;            // 2       |          |
    //                                            CCW  |          |
    indices[triIndex + 3] = idx;            // 2       |          |
    indices[triIndex + 4] = idx + res;      // 3       |          |
    indices[triIndex + 5] = idx + res + 1;  // 0       3 -------- 0
  };

  const auto appendIndices_PATCHES = [&] (size_t idx) {
    indices[triIndex + 0] = idx + res + 1; // 0
    indices[triIndex + 1] = idx + 1;       // 1
    indices[triIndex + 2] = idx;           // 2
    indices[triIndex + 3] = idx + res;     // 3
  };

  size_t indicesPerQuad = 0;
  std::function<void(size_t)> appendIndicesFunc;

  switch (sm->renderPrimitive) {
    case GL_TRIANGLES:
      indicesPerQuad = 6;
      appendIndicesFunc = appendIndices_TRIANGLES;
      break;
    case GL_PATCHES:
      indicesPerQuad = 4;
      appendIndicesFunc = appendIndices_PATCHES;
      break;
    default:
      error("[SphereFaceChunk::SphereFaceChunk] Unexpected render primitive [{}]", sm->renderPrimitive);
      break;
  }

  vertices.resize(res * res);
  indices.resize((res - 1) * (res - 1) * indicesPerQuad);

  vec3 axisA = vec3(up.y, up.z, up.x);
  vec3 axisB = cross(up, axisA);

  float step = 1.f / (sm->chunksPerSide * (res - 1));
  float globalStartY = start.y * (res - 1);
  float globalStartX = start.x * (res - 1);

  for (int y = 0; y < res; y++) {
    float globalY = globalStartY + y;
    float percentY = (1.f - globalY * step) * 2.f - 1.f; // "(1.f - ...)" CCW
    vec3 pY = percentY * axisB;

    for (int x = 0; x < res; x++) {
      int idx = x + y * res;
      float globalX = globalStartX + x;
      float percentX = globalX * step * 2.f - 1.f;
      vec3 pX = percentX * axisA;
      vec3 pointOnPlane = up + pX + pY;

      auto& vertex = vertices[idx];
      vertex.position = spherifyFancy(pointOnPlane) * sm->radius;
      vertex.texture = vec2{percentX, percentY} * 0.5f + 0.5f;

      if (x != res - 1 && y != res - 1) {
        appendIndicesFunc(idx);
        triIndex += indicesPerQuad;
      }
    }
  }

  MeshData data(vertices, indices);
  data.mode = sm->renderPrimitive;
  mesh = MeshElements(data);
}

vec3 SphereFaceChunk::spherify(const vec3& v) { return normalize(v); }

vec3 SphereFaceChunk::spherifyFancy(const vec3& v) {
  float x2 = v.x * v.x;
  float y2 = v.y * v.y;
  float z2 = v.z * v.z;

  return {
    v.x * sqrtf(1.f - (y2 + z2) * 0.5f + (y2 * z2) / 3.f),
    v.y * sqrtf(1.f - (z2 + x2) * 0.5f + (z2 * x2) / 3.f),
    v.z * sqrtf(1.f - (x2 + y2) * 0.5f + (x2 * y2) / 3.f),
  };
}

