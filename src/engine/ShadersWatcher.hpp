#pragma once

#include <unordered_set>

#include "Shader.hpp"

struct ShadersWatcher {
  void add(Shader* shader) {
    shaders.insert(shader);
  }

  bool check() {
    bool reloaded = false;
    for (Shader* shader : shaders)
      if ((reloaded |= shader->needsReload()))
        shader->reload();

    return reloaded;
  }

private:
  std::unordered_set<Shader*> shaders;
};

