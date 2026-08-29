#pragma once

#include "Camera.hpp"
#include "Shader.hpp"
#include "texture/TextureCubemap.hpp"

namespace environment {

extern TextureCubemap texEnvCubemapHDR;
extern TextureCubemap texIrradianceCubemapHDR;
extern fspath _lastLoadedImage;

void init();
void loadFromImageEquirectangularHDR(fspath hdrPath);
void update(bool forceUpdate = false);
void draw(const Camera* cam, Shader& shader); // Store shader here?

} // namespace environment

