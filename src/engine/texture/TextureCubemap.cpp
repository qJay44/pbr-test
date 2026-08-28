#include "TextureCubemap.hpp"

#include "../Shader.hpp"
#include "../FBO.hpp"
#include "../RBO.hpp"
#include "../Camera.hpp"
#include "../mesh/MeshElements.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

TextureCubemap TextureCubemap::convertEquirectangularHDR(const Texture2D& texHDR) {
  static Shader shaderConvert("equirectangular-hdr-to-cubema.vert", "equirectangular-hdr-to-cubema.frag");
  static FBO fboCapture{};
  static RBO rboCapture{};
  static MeshElements cube = MeshElements::loadFromOBJ("res/obj/Cube.obj");
  static Camera dummyCamera{vec3(0.f)};
  ivec2 size{1024};

  fboCapture.bind();
  rboCapture.bind();
  rboCapture.storage(GL_DEPTH_COMPONENT24, size.x, size.y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboCapture.id);

  TextureCubemap cubemap;
  cubemap.onInit({.target = GL_TEXTURE_CUBE_MAP});
  for (int i = 0; i < 6; i++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size.x, size.y, 0, GL_RGB, GL_FLOAT, nullptr);

  mat4 captureProj = glm::perspective(PI_2, 1.f, 0.1f, 10.f);
  mat4 captureViews[] = {
    glm::lookAt(vec3(0.f), vec3( 1.f,  0.f,  0.f), vec3(0.f, -1.f,  0.f)),
    glm::lookAt(vec3(0.f), vec3(-1.f,  0.f,  0.f), vec3(0.f, -1.f,  0.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f,  1.f,  0.f), vec3(0.f,  0.f,  1.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f, -1.f,  0.f), vec3(0.f,  0.f, -1.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f,  0.f,  1.f), vec3(0.f, -1.f,  0.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f,  0.f, -1.f), vec3(0.f, -1.f,  0.f)),
  };

  shaderConvert.use();
  shaderConvert.setUniformMatrix4f("u_proj", captureProj);
  texHDR.bind(0);
  glViewport(0, 0, size.x, size.y);
  fboCapture.bind();
  for (int i = 0; i < 6; i++) {
    shaderConvert.setUniformMatrix4f("u_view", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap.id, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.draw(&dummyCamera, shaderConvert);
  }
  fboCapture.unbind();

  return cubemap;
}

void TextureCubemap::loadFromImage(const fspath& path, const TextureDescriptor& desc) {
  loadFromImage(image2D(path), desc);
}

void TextureCubemap::loadFromImage(const image2D& img, const TextureDescriptor& desc) {
  onInit(desc);

  //     +Y
  //  -X +Z +X -Z
  //     -Y
  constexpr int skipCols[6] = {2, 0, 1, 1, 1, 3};
  constexpr int skipRows[6] = {1, 1, 0, 2, 1, 1};

  int faceSize = img.width / 4;
  glPixelStorei(GL_UNPACK_ROW_LENGTH, img.width);

  for (int i = 0; i < 6; i++) {
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, faceSize * skipCols[i]);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, faceSize * skipRows[i]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, desc.internalFormat, faceSize, faceSize, 0, desc.format, desc.type, img.pixels);
  }

  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  unbind();
}

void TextureCubemap::onInit(const TextureDescriptor& desc) {
  if (desc.target != GL_TEXTURE_CUBE_MAP)
    error("[TextureCubemap::TextureCubemap] Wrong target ({:#x})", desc.target);

  clear();
  target = desc.target;

  glGenTextures(1, &id);
  bind(0);
  glTexParameteri(desc.target, GL_TEXTURE_MIN_FILTER, desc.minFilter);
  glTexParameteri(desc.target, GL_TEXTURE_MAG_FILTER, desc.magFilter);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_S, desc.wrapS);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_T, desc.wrapT);
  glTexParameteri(desc.target, GL_TEXTURE_WRAP_R, desc.wrapR);
}

