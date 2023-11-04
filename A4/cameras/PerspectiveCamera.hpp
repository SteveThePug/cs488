#pragma once

#include "Camera.hpp"

class PerspectiveCamera : public Camera {
private:
  float fov, aspect;           // fov and aspect
  float near_z, far_z;         // Near and far planes
  glm::mat4 projection_matrix; // Calculated projection matrix

public:
  /** Initialise the perspective camera
   * @param fov Camera fov
   * @param aspect Camera aspect ratio
   * @param near_z Near plane
   * @param far_z Far plane
   */
  PerspectiveCamera(float fov, float aspect, float near_z, float far_z,
                    glm::vec3 positon, glm::vec3 target, glm::vec3 up);

  // Getters
  /** Return the projection matrix
   * @return perspective matrix
   */
  glm::mat4 getProjectionMatrix();

  // Setters
  /** Set the perspective of the camera
   * @param fov Camera fov
   * @param aspect Camera aspect ratio
   * @param near_z Near plane
   * @param far_z Far plane
   */
  void setPerspective(float fov, float aspect, float near_z, float far_z);

  /** Update the projection matrix off given parameters
   */
  void updateProjectionMatrix();

  /** Render the scene to the viewport
   * @param scene The scene to render
   * @returns Viewport width x height pixel array
   */
  void renderToViewport(Viewport *viewport, const SceneNode &scene) override;
};