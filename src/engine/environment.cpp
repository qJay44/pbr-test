#include "environment.hpp"

#include "Camera.hpp"
#include "FBO.hpp"
#include "RBO.hpp"
#include "Shader.hpp"
#include "ShadersWatcher.hpp"
#include "mesh/MeshElements.hpp"
#include "environment.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "mesh/MeshElements.hpp"
#include "texture/Texture2D.hpp"
#include "texture/TextureDescriptor.hpp"
#include "utils/utils.hpp"

namespace {

FBO fboCapture;
RBO rboCapture;
const Camera dummyCamera{vec3(0.f)};

void captureCubemapPass(const Texture* texRead, TextureCubemap& texWrite, Shader& shader, ivec2 resolution, GLint mipLevel = 0) {
  static const MeshElements cube = MeshElements::loadFromOBJ("res/obj/Cube.obj");

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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texWrite.getId(), mipLevel);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    cube.draw(&dummyCamera, shader);
  }
  fboCapture.unbind();
}

void captureScreenPass(Texture2D& texWrite, Shader& shader, ivec2 resolution) {
  fboCapture.bind();
  rboCapture.bind();
  rboCapture.storage(GL_DEPTH_COMPONENT24, resolution.x, resolution.y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboCapture.id);

  shader.use();
  glViewport(0, 0, resolution.x, resolution.y);
  fboCapture.bind();

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texWrite.getId(), 0);
  Mesh::drawScreen(&dummyCamera, shader);

  fboCapture.unbind();
}

} // namespace

namespace environment {

namespace {

constexpr TextureDescriptor generalTexDescHDR{
  .target = GL_NONE, // NOTE: Read members, never pass this descriptor as is
  .internalFormat = GL_RGB32F,
  .format = GL_RGB,
  .type = GL_FLOAT
};

constexpr ivec2 envCubemapResolution{1024};
constexpr ivec2 envPrefilterCubemapResolution{128};
constexpr ivec2 irradianceResolution{32};
constexpr ivec2 brdfLutResolution{512};

bool initialized = false;
MeshElements meshSkyboxCube;
Shader shaderConvert;
Shader shaderConvolute;
Shader shaderConvolutePrefilter;
Shader shaderConvoluteBrdf;
ShadersWatcher shadersWatcher;
Texture2D texEnvHDR;

void generateEnvironment() {
  captureCubemapPass(&texEnvHDR, texEnvCubemapHDR, shaderConvert, envCubemapResolution);
  texEnvCubemapHDR.bind(0);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void generateIrradiance() {
  captureCubemapPass(&texEnvCubemapHDR, texIrradianceCubemapHDR, shaderConvolute, irradianceResolution);
}

void generatePrefilter() {
  for (GLint mip = 0; mip < maxMipLevels; mip++) {
    ivec2 mipSize = vec2(envPrefilterCubemapResolution) * glm::pow(0.5f, (float)mip);
    float roughness = (float)mip / (maxMipLevels - 1.f);
    shaderConvolutePrefilter.setUniform1f("u_roughness", roughness);
    captureCubemapPass(&texEnvCubemapHDR, texEnvPrefilterCubemapHDR, shaderConvolutePrefilter, mipSize, mip);
  }
}

void generateBrdfLut() {
  captureScreenPass(texBrdfLut, shaderConvoluteBrdf, brdfLutResolution);
}

} // namespace

TextureCubemap texEnvCubemapHDR;
TextureCubemap texEnvPrefilterCubemapHDR;
TextureCubemap texIrradianceCubemapHDR;
Texture2D texBrdfLut;
GLint maxMipLevels = 5;
fspath _lastLoadedImage;

void init() {
  if (std::exchange(initialized, true))
    error("[environment::init] Already initialized");

  // ----- Buffer objects ---------------------------------------------------------------------------------------------------------- //

  fboCapture.gen();
  rboCapture.gen();

  // ----- Textures ---------------------------------------------------------------------------------------------------------------- //

  // NOTE: Global option
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  TextureDescriptor cubemapDesc = generalTexDescHDR;
  cubemapDesc.target = GL_TEXTURE_CUBE_MAP;
  texIrradianceCubemapHDR.initEmpty(irradianceResolution, cubemapDesc);

  cubemapDesc.minFilter = GL_LINEAR_MIPMAP_LINEAR;
  texEnvCubemapHDR.initEmpty(envCubemapResolution, cubemapDesc);

  texEnvPrefilterCubemapHDR.initEmpty(envPrefilterCubemapResolution, cubemapDesc);
  texEnvPrefilterCubemapHDR.bind(0);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  texEnvPrefilterCubemapHDR.unbind();

  texBrdfLut.initStorage(brdfLutResolution, {.internalFormat = GL_RG16, .format = GL_RG, .type = GL_FLOAT});

  // ----- Shaders ----------------------------------------------------------------------------------------------------------------- //

  shaderConvert = Shader("cubemap-capture/cubemap.vert", "cubemap-capture/equirectangular-hdr-to-cubemap.frag");
  shaderConvolute = Shader("cubemap-capture/cubemap.vert", "cubemap-capture/convolute.frag");
  shaderConvolutePrefilter = Shader("cubemap-capture/cubemap.vert", "cubemap-capture/convolute-prefilter.frag");
  shaderConvoluteBrdf = Shader("ndc.vert", "convolute-brdf.frag");

  shadersWatcher.add(&shaderConvert);
  shadersWatcher.add(&shaderConvolute);
  shadersWatcher.add(&shaderConvolutePrefilter);
  shadersWatcher.add(&shaderConvoluteBrdf);

  // ----- Other ------------------------------------------------------------------------------------------------------------------- //

  meshSkyboxCube = MeshElements::loadFromOBJ("res/obj/Cube.obj");
}

void loadFromImageEquirectangularHDR(fspath hdrPath) {
  assert(initialized);

  _lastLoadedImage = hdrPath;

  TextureDescriptor texDesc = generalTexDescHDR;
  texDesc.target = GL_TEXTURE_2D;
  texEnvHDR = Texture2D(image2D(hdrPath, true, IMAGE2D_LOAD_STBF), texDesc);

  update(true);
}

void update(bool forceUpdate) {
  if (shadersWatcher.check() || forceUpdate) {
    generateEnvironment();
    generateIrradiance();
    generatePrefilter();
    generateBrdfLut();
  }
}

void draw(const Camera* cam, Shader& shader) {
  assert(initialized);

  glDepthFunc(GL_LEQUAL);
  glDisable(GL_CULL_FACE);
  texEnvCubemapHDR.bind(0);
  meshSkyboxCube.draw(cam, shader);

  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
}

} // namespace environment

