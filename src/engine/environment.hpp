#pragma once

#include "Camera.hpp"
#include "Shader.hpp"
#include "texture/Texture2D.hpp"
#include "texture/TextureCubemap.hpp"

namespace environment {

extern TextureCubemap texEnvCubemapHDR;
extern TextureCubemap texEnvPrefilterCubemapHDR;
extern TextureCubemap texIrradianceCubemapHDR;
extern Texture2D texBrdfLut;
extern fspath _lastLoadedImage;

void init();
void loadFromImageEquirectangularHDR(fspath hdrPath);
void update(bool forceUpdate = false);
void draw(const Camera* cam, Shader& shader); // Store shader here?

} // namespace environment

