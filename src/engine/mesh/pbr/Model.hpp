#pragma once

#include "../pbr/Material.hpp"
#include "../../environment.hpp"

namespace pbr {

struct Model {
  pbr::Material material{};

  void preDraw(Shader& shader) const {
    material.setUniforms(shader);
    material.bindTextures();

    shader.setUniform1f("u_maxReflectionLod", environment::maxMipLevels);

    environment::texBrdfLut.bind(7);
    environment::texEnvPrefilterCubemapHDR.bind(8);
    environment::texIrradianceCubemapHDR.bind(9);
  }
};

}

