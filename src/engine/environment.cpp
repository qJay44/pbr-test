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

namespace {

void capturePass(const Texture* texRead, TextureCubemap& texWrite, Shader& shader, ivec2 resolution) {
  static const FBO fboCapture{};
  static const RBO rboCapture{};
  static const MeshElements cube = MeshElements::loadFromOBJ("res/obj/Cube.obj");
  static const Camera dummyCamera{vec3(0.f)};

  static const mat4 captureProj = glm::perspective(PI_2, 1.f, 0.1f, 10.f);
  static const mat4 captureViews[] = {
    glm::lookAt(vec3(0.f), vec3( 1.f,  0.f,  0.f), vec3(0.f, -1.f,  0.f)),
    glm::lookAt(vec3(0.f), vec3(-1.f,  0.f,  0.f), vec3(0.f, -1.f,  0.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f,  1.f,  0.f), vec3(0.f,  0.f,  1.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f, -1.f,  0.f), vec3(0.f,  0.f, -1.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f,  0.f,  1.f), vec3(0.f, -1.f,  0.f)),
    glm::lookAt(vec3(0.f), vec3( 0.f,  0.f, -1.f), vec3(0.f, -1.f,  0.f)),
  };

  fboCapture.bind();
  rboCapture.bind();
  rboCapture.storage(GL_DEPTH_COMPONENT24, resolution.x, resolution.y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboCapture.id);

  shader.use();
  shader.setUniformMatrix4f("u_proj", captureProj);
  texRead->bind(0);
  glViewport(0, 0, resolution.x, resolution.y);
  fboCapture.bind();
  for (int i = 0; i < 6; i++) {
    shader.setUniformMatrix4f("u_view", captureViews[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texWrite.getId(), 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.draw(&dummyCamera, shader);
  }
  fboCapture.unbind();
}

} // namespace

namespace environment {

namespace {

bool initialized = false;
MeshElements meshSkyboxCube;
Shader shaderConvert;
Shader shaderConvolute;

constexpr TextureDescriptor generalTexDescHDR{
  .target = GL_NONE, // NOTE: Read members, never pass this descriptor as is
  .internalFormat = GL_RGB32F,
  .format = GL_RGB,
  .type = GL_FLOAT
};

constexpr ivec2 cubemapResolution{1024};
constexpr ivec2 irradianceResolution{32};

} // namespace

TextureCubemap texEnvCubemapHDR;
TextureCubemap texIrradianceCubemapHDR;
fspath _lastLoadedImage;

void init() {
  if (initialized)
    error("[environment::init] Already initialized");

  TextureDescriptor cubemapDesc = generalTexDescHDR;
  cubemapDesc.target = GL_TEXTURE_CUBE_MAP;
  texEnvCubemapHDR.initEmpty(cubemapResolution, cubemapDesc);
  texIrradianceCubemapHDR.initEmpty(irradianceResolution, cubemapDesc);

  meshSkyboxCube = MeshElements::loadFromOBJ("res/obj/Cube.obj");
  initialized = true;
}

void loadFromImageEquirectangularHDR(fspath hdrPath) {
  assert(initialized);

  _lastLoadedImage = hdrPath;

  TextureDescriptor texDesc = generalTexDescHDR;
  texDesc.target = GL_TEXTURE_2D;
  Texture2D texEnvHDR(image2D(hdrPath, IMAGE2D_LOAD_STBF, true), texDesc);

  shaderConvert = Shader("equirectangular-hdr-to-cubemap.vert", "equirectangular-hdr-to-cubemap.frag");
  shaderConvolute = Shader("convolute-cubemap.vert", "convolute-cubemap.frag");

  capturePass(&texEnvHDR, texEnvCubemapHDR, shaderConvert, cubemapResolution);
  capturePass(&texEnvCubemapHDR, texIrradianceCubemapHDR, shaderConvolute, irradianceResolution);
}

void draw(const Camera* cam, Shader& shader) {
  assert(initialized);

  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  // texEnvCubemapHDR.bind(0);
  texIrradianceCubemapHDR.bind(0);
  meshSkyboxCube.draw(cam, shader);

  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
}

} // namespace environment

