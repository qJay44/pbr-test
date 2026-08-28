#include "environment.hpp"

#include "mesh/MeshElements.hpp"
#include "texture/Texture2D.hpp"
#include "utils/utils.hpp"
#include <cassert>

namespace environment {

namespace {

bool initialized = false;
MeshElements meshSkyboxCube;

}

TextureCubemap texEnvCubemap;
fspath _lastLoadedImage;

void init() {
  if (initialized)
    error("[environment::init] Already initialized");

  meshSkyboxCube = MeshElements::loadFromOBJ("res/obj/Cube.obj");
  initialized = true;
}

void loadFromImageEquirectangularHDR(fspath hdrPath) {
  assert(initialized);

  Texture2D texEnvHDR(image2D(hdrPath, IMAGE2D_LOAD_STBF, true), {.internalFormat = GL_RGB32F, .type = GL_FLOAT});
  texEnvCubemap = TextureCubemap::convertEquirectangularHDR(texEnvHDR);
  _lastLoadedImage = hdrPath;
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

