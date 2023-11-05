// Camera.cpp
#include "Camera.hpp"
#include <glm/gtx/transform.hpp> // For glm::lookAt

// Constructor and other necessary methods if needed

Camera::Camera(glm::vec3 position, glm::vec3 view, glm::vec3 up, bool target)
    : position(position), up(up) {
  if (target) {
    this->view = glm::normalize(view - position);
  } else {
    this->view = glm::normalize(view);
  }
  updateMatricies();
}

// Set camera position
void Camera::setPosition(const glm::vec3 &position) {
  this->position = position;
  updateMatricies();
}

// Set camera target
void Camera::setView(const glm::vec3 &view, bool target) {
  if (target) {
    this->view = glm::normalize(view - position);
  } else {
    this->view = glm::normalize(view);
  }
  updateMatricies();
}

// Set up vector
void Camera::setUp(const glm::vec3 &up) { this->up = up; }

// Get camera position
glm::vec3 Camera::getPosition() const { return this->position; }

// Get target
glm::vec3 Camera::getView() const { return this->view; }

// Get camera view matrix
glm::mat4 Camera::getViewMatrix() const {
  // Implement method
  return this->view_matrix;
}

// Update the view matrix
void Camera::updateViewMatrix() {
  this->view_matrix = glm::lookAt(position, position + view, up);
}

// Update all respective camera matricies
void Camera::updateMatricies() { updateViewMatrix(); }
