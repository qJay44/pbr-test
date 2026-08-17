#include "Camera.hpp"

#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/matrix.hpp"
#include "glm/trigonometric.hpp"

#include "mesh/meshes.hpp"
#include "global.hpp"

Camera::Camera(vec3 pos, float yaw, float pitch) : Moveable(pos, yaw, pitch) {
  update();
};

const float& Camera::getNearPlane()   const { return nearPlane;   }
const float& Camera::getFarPlane()    const { return farPlane;    }
const float& Camera::getFov()         const { return fov;         }
const float& Camera::getAspectRatio() const { return aspectRatio; }
const mat4&  Camera::getProj()        const { return proj;        }
const mat4&  Camera::getView()        const { return view;        }
const mat4&  Camera::getProjView()    const { return pv;          }

mat4 Camera::getProjViewInv() const {
  return glm::inverse(pv);
}

void Camera::setNearPlane(float p) { nearPlane = p; }
void Camera::setFarPlane(float p) { farPlane = p; }
void Camera::setFlags(u32 f) { flags = f; }

void Camera::setUniforms(Shader& s) const {
  s.setUniform1f      ("u_camNear"   , getNearPlane());
  s.setUniform1f      ("u_camFar"    , getFarPlane());
  s.setUniform1f      ("u_camFov"    , getFov());
  s.setUniform3f      ("u_camPos"    , getPosition());
  s.setUniform3f      ("u_camRight"  , getRight());
  s.setUniform3f      ("u_camUp"     , getUp());
  s.setUniform3f      ("u_camForward", getForward());
  s.setUniformMatrix4f("u_camProj"   , getProj());
  s.setUniformMatrix4f("u_camView"   , getView());
  s.setUniformMatrix4f("u_camPV"     , getProjView());
}

void Camera::update() {
  vec2 winSize = global::getWinSize();

  aspectRatio = winSize.x / winSize.y;
  proj = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
  view = glm::lookAt(position, position + orientation, up);
  pv = proj * view;

  dvec2 winCenter = global::getWinCenter();
  if (!global::guiFocused)
    glfwSetCursorPos(global::window, winCenter.x, winCenter.y);
}

void Camera::draw(const Camera* cam) const {
  if (this != cam) {
    const vec3& p = position;

    if (flags & CameraFlags_DrawRight)   Mesh::drawDebugDirectionLine(cam, p, getRight(), global::red);
    if (flags & CameraFlags_DrawUp)      Mesh::drawDebugDirectionLine(cam, p, up, global::green);
    if (flags & CameraFlags_DrawForward) Mesh::drawDebugDirectionLine(cam, p, orientation, global::blue);

    if (flags & CameraFlags_DrawFrustum) {
      MeshData frustumMeshData = meshes::frustumMeshData(this);
      static MeshElements frustumMesh(frustumMeshData);
      frustumMesh.updateBufferVBO(frustumMeshData);

      static Shader shader("debug/lines.vert", "debug/lines.frag");

      shader.setUniform3f("u_color", vec3(1.f));
      frustumMesh.draw(cam, shader);
    }
  }
}

