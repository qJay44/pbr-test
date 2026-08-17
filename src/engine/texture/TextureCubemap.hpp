#pragma once

#include "Texture.hpp"
#include "TextureDescriptor.hpp"
#include "image2D.hpp"

class TextureCubemap : public Texture {
public:
  using Texture::Texture;

  TextureCubemap() = default;

  void loadFromImage(const fspath& path, const TextureDescriptor& desc);
  void loadFromImage(const image2D& img, const TextureDescriptor& desc);
private:
  void onInit(const TextureDescriptor& desc);
};

