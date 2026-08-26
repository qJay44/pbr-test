#pragma once

#include <array>
#include <filesystem>
#include <string>

#include "../Shader.hpp"
#include "../texture/Texture2D.hpp"
#include "f0.hpp"
#include "utils/utils.hpp"

struct Material {
  enum TextureIdx {
    IDX_ALBEDO,
    IDX_AO,
    IDX_EMISSIVITY,
    IDX_HEIGHT,
    IDX_METALLIC,
    IDX_NORMAL,
    IDX_ROUGHNESS,
  };

  f0::index baseReflectivityIdx = f0::IDX_IRON;
  std::array<Texture2D, 7> textures{};
  fspath currFolder;

  void loadFrom(fspath folderPath) {
    namespace fs = std::filesystem;
    using enum TextureIdx;

    if (fs::exists(folderPath) && fs::is_directory(folderPath)) {
      std::string prefixName = folderPath.lexically_normal().filename().string() + "_";
      const auto path_to = [&prefixName, &folderPath] (const std::string& f) {
        return folderPath / (prefixName + f);
      };

      TextureDescriptor descRGBA{.internalFormat = GL_RGBA8, .format = GL_RGBA, .wrapS = GL_REPEAT, .wrapT = GL_REPEAT};
      TextureDescriptor descRGB{.wrapS = GL_REPEAT, .wrapT = GL_REPEAT};
      TextureDescriptor descRed{.internalFormat = GL_R8, .wrapS = GL_REPEAT, .wrapT = GL_REPEAT};

      textures[IDX_ALBEDO]     = Texture2D(image2D(path_to("albedo.png")),    descRGB);
      textures[IDX_AO]         = Texture2D(image2D(path_to("ao.png")),        descRed);
      textures[IDX_HEIGHT]     = Texture2D(image2D(path_to("height.png")),    descRed);
      textures[IDX_METALLIC]   = Texture2D(image2D(path_to("metallic.png")),  descRed);
      textures[IDX_NORMAL]     = Texture2D(image2D(path_to("normal-dx.png")), descRGB);
      textures[IDX_ROUGHNESS]  = Texture2D(image2D(path_to("roughness.png")), descRed);

      fspath emissivePath = path_to("emissive.png");
      if (fs::exists(emissivePath))
        textures[IDX_EMISSIVITY] = Texture2D(image2D(emissivePath),  descRGBA);
      else
        textures[IDX_EMISSIVITY].initStorage({1, 1}, {});

      currFolder = folderPath;

    } else {
      error("[Material::loadFrom] Provided folder ({}) is invalid", folderPath.string());
    }
  }

  void setUniforms(Shader& shader) const {
    shader.setUniform3f("u_baseReflectivity", f0::all[baseReflectivityIdx].color);
  }

  void bindTextures() const {
    for (size_t i = 0; i < textures.size(); i++)
      textures[i].bind(i);
  }
};

