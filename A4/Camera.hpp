#pragma once

#include "glm/glm.hpp"

class Camera {
private:
  glm::vec3 position;    // Position of camera in world space
  glm::vec3 target;      // Direction camera is looking
  glm::vec3 up;          // Up vector
  glm::mat4 view_matrix; // Calculated view matrix

public:
  // Constructor
  /** Constructor for the class
   * @param position position of the camera
   * @param target target camera is looking at
   * @param up up vector
   */
  Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);
  virtual ~Camera() = default;

  // Setters
  /** Set camera position
   * @param position Camera's position
   */
  void setPosition(const glm::vec3 &position);

  /** Set camera orientation
   * @param orientation Camera's orientation
   */
  void setTarget(const glm::vec3 &target);

  /** Set the up vector
   * @param up The up vector
   */
  void setUp(const glm::vec3 &up);

  // Getters

  /** returns camera position
   * @return camera position
   */
  glm::vec3 getPosition();
  /** returns camera view matrix
   * @return view matrix
   */
  glm::mat4 getViewMatrix();
  /** returns camera projection matrix
   * @return projection matrix
   */
  virtual glm::mat4 getProjectionMatrix() = 0;

  // Updates

  /** Generates a view matrix based on field variables */
  virtual void updateProjectionMatrix() = 0;
  /** Generates a projection matrix based on field variables */
  void updateViewMatrix();
  /** Updates the camera view and projection matrix */
  void updateMatricies();
};
