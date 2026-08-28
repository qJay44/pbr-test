#pragma once

#include "Camera.hpp"
#include "Shader.hpp"
#include "texture/TextureCubemap.hpp"

namespace environment {

extern TextureCubemap texEnvCubemap;
extern fspath _lastLoadedImage;

void init();
void loadFromImageEquirectangularHDR(fspath hdrPath);
void draw(const Camera* cam, Shader& shader); // Store shader here?

} // namespace environment

