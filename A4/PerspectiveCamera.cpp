#include "PerspectiveCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near_z,
                                     float far_z, glm::vec3 position,
                                     glm::vec3 target, glm::vec3 up)
    : Camera(position, target, up) {
  setPerspective(fov, aspect, near_z, far_z);
  updateProjectionMatrix();
}

void PerspectiveCamera::setPerspective(float fov, float aspect, float near_z,
                                       float far_z) {
  assert(fov > 0.0f && fov < 180.0f);
  assert(aspect > 0.0f);
  assert(near_z > 0.0f && near_z < far_z);
  assert(far_z > near_z);

  this->fov = fov;
  this->aspect = aspect;
  this->near_z = near_z;
  this->far_z = far_z;
  return;
}

void PerspectiveCamera::updateProjectionMatrix() {
  projection_matrix = glm::perspective(fov, aspect, near_z, far_z);
}

glm::mat4 PerspectiveCamera::getProjectionMatrix() { return projection_matrix; }

void PerspectiveCamera::renderToViewport(Viewport *viewport,
                                         const SceneNode &sceneNode) {
  return;
}
