#pragma once

#include "Camera.hpp"
#include "PerspectiveCamera.hpp"
#include "Triangle.hpp"

class RayCamera : public Camera {
private:
  float fov, aspect;

  // From Camera.hpp
  //  glm::vec3 position;    // Position of camera in world space
  //  glm::vec3 target;      // Direction camera is looking
  //  glm::vec3 up;          // Up vector
  //  glm::mat4 view_matrix; // Calculated view matrix
public:
  /** Constructor for the RayCamera
   * @param fov
   * @param aspect
   * @param position
   * @param target
   * @param up
   * @returns RayCamera
   */
  RayCamera(float fov, float aspect, glm::vec3 position, glm::vec3 target,
            glm::vec3 up);

  /** Temporary function to render a triangle to a viewport
   * @param viewport Viewport to render to
   * @param tri Triangle to render
   */
  void renderTriangleToViewport(Viewport *viewport, const Triangle &tri);

  /** Render the scene to the viewport
   * @param scene The scene to render
   * @returns Viewport width x height pixel array
   */
  void renderToViewport(Viewport *viewport, SceneNode *scene) override;
};
