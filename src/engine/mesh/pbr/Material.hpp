#pragma once

#include <array>
#include <filesystem>
#include <string>

#include "../../Shader.hpp"
#include "../../texture/Texture2D.hpp"
#include "f0.hpp"
#include "utils/utils.hpp"

namespace pbr {

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

  struct Custom {
    vec3 albedo;
    vec3 emissivity;
    vec3 normal;
    float metallic;
    float roughness;
    float ao;
  };

  f0::index baseReflectivityIdx = f0::IDX_IRON;
  std::array<Texture2D, 7> textures{};
  fspath currFolder;
  Custom customMaterial{};
  bool useCustomMaterial = false;

  Material() {
    for (auto& tex : textures)
      tex.initStorage(image2D{1, 1}, {});
  }

  void loadFrom(fspath folderPath, bool flipVertically = true) {
    namespace fs = std::filesystem;
    using enum TextureIdx;

    if (fs::exists(folderPath) && fs::is_directory(folderPath)) {
      std::string ext;
      {
        // NOTE: Assuming all images have same extension
        auto it = fs::directory_iterator(folderPath);
        if (it != fs::directory_iterator{})
          ext = it->path().extension();
        else
          error("[Material::loadFrom] Directory ({}) is empty", folderPath.string());
      }

      std::string prefixName = folderPath.lexically_normal().filename().string() + "_";

      const auto path_to = [&prefixName, &folderPath] (const std::string& f) {
        return folderPath / (prefixName + f);
      };

      TextureDescriptor descSRGBA{.internalFormat = GL_SRGB_ALPHA, .format = GL_RGBA, .wrapS = GL_REPEAT, .wrapT = GL_REPEAT};
      TextureDescriptor descRGB{.wrapS = GL_REPEAT, .wrapT = GL_REPEAT};
      TextureDescriptor descSRGB{.internalFormat = GL_SRGB8, .wrapS = GL_REPEAT, .wrapT = GL_REPEAT};
      TextureDescriptor descRed{.internalFormat = GL_R8, .wrapS = GL_REPEAT, .wrapT = GL_REPEAT};

      textures[IDX_ALBEDO]     = Texture2D(image2D(path_to("albedo"    + ext), flipVertically), descSRGB);
      textures[IDX_AO]         = Texture2D(image2D(path_to("ao"        + ext), flipVertically), descRed);
      textures[IDX_METALLIC]   = Texture2D(image2D(path_to("metallic"  + ext), flipVertically), descRed);
      textures[IDX_NORMAL]     = Texture2D(image2D(path_to("normal-dx" + ext), flipVertically), descRGB);
      textures[IDX_ROUGHNESS]  = Texture2D(image2D(path_to("roughness" + ext), flipVertically), descRed);

      fspath emissivePath = path_to("emissive" + ext);
      fspath heightPath = path_to("height" + ext);

      if (fs::exists(emissivePath))
        textures[IDX_EMISSIVITY] = Texture2D(image2D(emissivePath, flipVertically),  descSRGBA);

      if (fs::exists(heightPath))
        textures[IDX_HEIGHT] = Texture2D(image2D(heightPath, flipVertically),  descRed);

      currFolder = folderPath;

    } else {
      error("[Material::loadFrom] Provided folder ({}) is invalid", folderPath.string());
    }
  }

  void setUniforms(Shader& shader) const {
    shader.setUniform3f("u_baseReflectivity", f0::all[baseReflectivityIdx].color);
    shader.setUniform1i("u_useCustomMaterial", useCustomMaterial);

    if (useCustomMaterial) {
      shader.setUniform3f("u_customMaterial.albedo", customMaterial.albedo);
      shader.setUniform3f("u_customMaterial.emissivity", customMaterial.emissivity);
      shader.setUniform3f("u_customMaterial.normal", customMaterial.normal);
      shader.setUniform1f("u_customMaterial.metallic", customMaterial.metallic);
      shader.setUniform1f("u_customMaterial.roughness", customMaterial.roughness);
      shader.setUniform1f("u_customMaterial.ao", customMaterial.ao);
    }
  }

  void bindTextures() const {
    if (!useCustomMaterial)
      for (size_t i = 0; i < textures.size(); i++)
        textures[i].bind(i);
  }
};

}

