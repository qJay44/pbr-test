#include "Light.hpp"

Light::Light(vec3 position, float radius, vec3 color)
  : position(position), radius(radius), color(color) {}

const vec3& Light::getPosition() const { return position; }
const vec3& Light::getColor() const { return color; }

void Light::update() {
  mesh.setMatTranslation(position);
}

void Light::setUniforms(Shader& shader) const {
  shader.setUniform1f("u_lightRadius", radius);
  shader.setUniform3f("u_lightPos", position);
  shader.setUniform3f("u_lightColor", color);
}

void Light::draw(const Camera* camera, Shader& shader) const {
  setUniforms(shader);

  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  mesh.draw(camera, shader);

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

