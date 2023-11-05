#pragma once

#include "SceneNode.hpp"
#include "Viewport.hpp"
#include "glm/glm.hpp"

class Camera {
protected:
  glm::vec3 position;    // Position of camera in world space
  glm::vec3 view;        // Direction camera is looking
  glm::vec3 up;          // Up vector
  glm::mat4 view_matrix; // Calculated view matrix

public:
  // Constructor
  /** Constructor for the class
   * @param position position of the camera
   * @param target target camera is looking at
   * @param up up vector
   */
  Camera(glm::vec3 position, glm::vec3 viewORtarget, glm::vec3 up,
         bool target = false);

  // Setters
  /** Set camera position
   * @param position Camera's position
   */
  void setPosition(const glm::vec3 &position);

  /** Set camera orientation
   * @param orientation Camera's orientation
   */
  void setView(const glm::vec3 &view, bool target = false);

  /** Set the up vector
   * @param up The up vector
   */
  void setUp(const glm::vec3 &up);

  // Getters

  /** returns camera position
   * @return camera position
   */
  glm::vec3 getPosition() const;
  /**
   * returns camera's target
   * @return camera target
   */
  glm::vec3 getView() const;
  /** returns camera view matrix
   * @return view matrix
   */
  glm::mat4 getViewMatrix() const;

  // Updates
  /** Generates a projection matrix based on field variables */
  void updateViewMatrix();
  /** Updates the camera view and projection matrix */
  void updateMatricies();
};
