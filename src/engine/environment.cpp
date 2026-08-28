#include "environment.hpp"

#include "Camera.hpp"
#include "FBO.hpp"
#include "RBO.hpp"
#include "Shader.hpp"
#include "mesh/MeshElements.hpp"
#include "environment.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "mesh/MeshElements.hpp"
#include "texture/Texture2D.hpp"
#include "texture/TextureDescriptor.hpp"
#include "utils/utils.hpp"

namespace environment {

namespace {

bool initialized = false;
MeshElements meshSkyboxCube;

constexpr TextureDescriptor generarTexDesc{
  .target = GL_NONE, // NOTE: Read members, never pass this descriptor as is
  .internalFormat = GL_RGB32F,
  .format = GL_RGB,
  .type = GL_FLOAT
};

constexpr ivec2 cubemapResolution{1024};

void convertEquirectangularHDR(const Texture2D& texHDR) {
  static Shader shaderConvert("equirectangular-hdr-to-cubemap.vert", "equirectangular-hdr-to-cubemap.frag");
  static FBO fboCapture{};
  static RBO rboCapture{};
  static MeshElements cube = MeshElements::loadFromOBJ("res/obj/Cube.obj");
  static Camera dummyCamera{vec3(0.f)};

  fboCapture.bind();
  rboCapture.bind();
  rboCapture.storage(GL_DEPTH_COMPONENT24, cubemapResolution.x, cubemapResolution.y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboCapture.id);

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
  glViewport(0, 0, cubemapResolution.x, cubemapResolution.y);
  fboCapture.bind();
  for (int i = 0; i < 6; i++) {
    shaderConvert.setUniformMatrix4f("u_view", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texEnvCubemap.getId(), 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.draw(&dummyCamera, shaderConvert);
  }
  fboCapture.unbind();
}

} // namespace

TextureCubemap texEnvCubemap;
fspath _lastLoadedImage;

void init() {
  if (initialized)
    error("[environment::init] Already initialized");

  TextureDescriptor cubemapDesc = generarTexDesc;
  cubemapDesc.target = GL_TEXTURE_CUBE_MAP;
  texEnvCubemap.initEmpty(cubemapResolution, cubemapDesc);

  meshSkyboxCube = MeshElements::loadFromOBJ("res/obj/Cube.obj");
  initialized = true;
}

void loadFromImageEquirectangularHDR(fspath hdrPath) {
  assert(initialized);

  _lastLoadedImage = hdrPath;

  TextureDescriptor texDesc = generarTexDesc;
  texDesc.target = GL_TEXTURE_2D;
  Texture2D texEnvHDR(image2D(hdrPath, IMAGE2D_LOAD_STBF, true), texDesc);

  convertEquirectangularHDR(texEnvHDR);
}

void draw(const Camera* cam, Shader& shader) {
  assert(initialized);

  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  texEnvCubemap.bind(0);
  meshSkyboxCube.draw(cam, shader);

  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
}

} // namespace environment

