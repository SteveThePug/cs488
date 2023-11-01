#include "OrthographicCamera.hpp"
#include <glm/ext/matrix_clip_space.hpp>

OrthographicCamera::OrthographicCamera(float width, float height, float depth,
                                       glm::vec3 position, glm::vec3 target,
                                       glm::vec3 up)
    : Camera(position, target, up) {
  this->setPerspective(width, height, depth);
}

// Getters
glm::mat4 OrthographicCamera::getProjectionMatrix() {
  return this->projection_matrix;
}

// Setters
void OrthographicCamera::setPerspective(float width, float height,
                                        float depth) {
  this->width = width;
  this->height = height;
  this->depth = depth;
  updateProjectionMatrix();
}

void OrthographicCamera::updateProjectionMatrix() {
  float halfWidth = width / 2.0f;
  float halfHeight = height / 2.0f;
  projection_matrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                                 -depth / 2.0f, depth / 2.0f);
}
