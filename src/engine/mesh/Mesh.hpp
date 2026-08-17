#pragma once

#include "MeshData.hpp"
#include "Transformable.hpp"
#include "VAO.hpp"
#include "BufferObject.hpp"
#include "../Camera.hpp"
#include "../Shader.hpp"
#include "vertex.hpp"

class Mesh : public Transformable {
public:
  Mesh(const Mesh&) = delete;
  Mesh(Mesh&&) = default;

  Mesh& operator=(const Mesh&) = delete;
  Mesh& operator=(Mesh&&) = default;
  ~Mesh() = default;

  static void linkAttributes(const vertex::Layout& layout);

  static void drawScreen(const Camera* camera, Shader& shader);
  static void drawDebugDirectionLine(const Camera* camera, const vec3& p, const vec3& d, const vec3& color);

  GLsizei getElementCount() const;

  void updateBufferVBO(const MeshData& data);

  virtual void draw(const Camera* camera, Shader& shader) const = 0;
  virtual void draw(const Camera* camera, Shader& shader, const mat4& model) const = 0;

protected:
  GLenum mode;
  GLsizei elementCount;
  VAO vao{};
  BufferObject vbo{GL_ARRAY_BUFFER};

protected:
  Mesh() = default;

  static void setGlobalUniforms(Shader& s);
};

