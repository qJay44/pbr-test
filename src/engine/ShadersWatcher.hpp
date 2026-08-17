#pragma once

#include <unordered_set>

#include "Shader.hpp"

struct ShadersWatcher {
  ShadersWatcher() = delete;

  static void add(Shader* shader) {
    shaders.insert(shader);
  }

  static void check() {
    for (Shader* shader : shaders)
      if (shader->needsReload())
        shader->reload();
  }

private:
  static std::unordered_set<Shader*> shaders;
};

