#include "Light.hpp"

Light::Light(vec3 position, vec3 colorGammaSpace, vec3 multiplier, float radius)
  : position(position), color(glm::pow(colorGammaSpace, vec3(2.2f))), multiplier(multiplier), radius(radius) {}

const vec3& Light::getPosition() const { return position; }
const vec3& Light::getColor() const { return color; }

void Light::update() {
  mesh.setMatTranslation(position);
}

void Light::setUniforms(Shader& shader) const {
  shader.setUniform3f("u_light.pos", position);
  shader.setUniform3f("u_light.color", color);
  shader.setUniform3f("u_light.multiplier", multiplier);
  shader.setUniform1f("u_light.radius", radius);
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

