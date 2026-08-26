#include "LightPoint.hpp"

#include "glm/gtc/quaternion.hpp"

size_t LightPoint::nextId = 0;

LightPoint::LightPoint(vec3 position, vec3 colorGammaSpace, vec3 multiplier, float radius)
    : id(nextId++), position(position),
      color(glm::pow(colorGammaSpace, vec3(2.2f))), multiplier(multiplier),
      radius(radius) {}

const vec3& LightPoint::getPosition() const { return position; }
const vec3& LightPoint::getColor() const { return color; }

void LightPoint::update() {
  mesh.setMatTranslation(position);
}

void LightPoint::setUniforms(Shader& shader) const {
  shader.setUniform3f(std::format("u_light{}.pos", id), position);
  shader.setUniform3f(std::format("u_light{}.color", id), color);
  shader.setUniform3f(std::format("u_light{}.multiplier", id), multiplier);
  shader.setUniform1f(std::format("u_light{}.radius", id), radius);
}

void LightPoint::setUniformsNoId(Shader& shader) const {
  shader.setUniform3f("u_light.pos", position);
  shader.setUniform3f("u_light.color", color);
  shader.setUniform3f("u_light.multiplier", multiplier);
  shader.setUniform1f("u_light.radius", radius);
}

void LightPoint::draw(const Camera* camera, Shader& shader) const {
  setUniformsNoId(shader);

  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  mesh.draw(camera, shader);

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

