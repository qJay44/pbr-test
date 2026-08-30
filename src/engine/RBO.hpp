#pragma once

struct RBO {
  GLsizei size = 0;
  GLuint id = 0;

  RBO() = default;

  RBO(GLsizei size) {
    gen(size);
  }

  RBO(const RBO&) = delete;
  RBO& operator=(const RBO&) = delete;

  RBO(RBO&& other) {
    std::swap(id, other.id);
    std::swap(size, other.size);
  }

  RBO& operator=(RBO&& other) {
    if (this != &other) {
      std::swap(id, other.id);
      std::swap(size, other.size);
    }

    return *this;
  }

  ~RBO() {
    clear();
  }

  static void unbind() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }

  void gen(GLsizei size = 1) {
    if (id)
      error("[RBO::gen] RBO is already generated");

    this->size = size;
    glGenRenderbuffers(size, &id);
  }

  void bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, id);
  }

  void storage(GLenum internalFormat, GLsizei width, GLsizei height) const {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
    unbind();
  }

  void clear() {
    if (id) glDeleteRenderbuffers(size, &id);
    id = 0;
    size = 0;
  }
};

