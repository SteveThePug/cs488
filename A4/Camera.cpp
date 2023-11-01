// Camera.cpp
#include "Camera.hpp"
#include <glm/gtx/transform.hpp> // For glm::lookAt

// Constructor and other necessary methods if needed

// Set camera position
void Camera::setPosition(const glm::vec3 &position) {
  this->position = position;
  updateMatricies();
}

// Set camera target
void Camera::setTarget(const glm::vec3 &target) {
  this->target = target;
  updateMatricies();
}

// Set up vector
void Camera::setUp(const glm::vec3 &up) { this->up = up; }

// Get camera position
glm::vec3 Camera::getPosition() { return this->position; }

// Get camera view matrix
glm::mat4 Camera::getViewMatrix() {
  // Implement method
  return this->view_matrix;
}

// Update the view matrix
void Camera::updateViewMatrix() {
  view_matrix = glm::lookAt(position, target, up);
}

// Update all respective camera matricies
void Camera::updateMatricies() { updateViewMatrix(); }
