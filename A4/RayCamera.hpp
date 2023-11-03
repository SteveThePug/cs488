#pragma once

#include "Camera.hpp"
#include "Triangle.hpp"
#include "Viewport.hpp"

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
   * @param fov Fov of camera
   * @param aspect aspect of camera
   * @param position position of camera
   * @param target target of camera
   * @param up up vector for camera
   * @returns RayCamera the camera
   */
  RayCamera(float fov, float aspect, glm::vec3 position, glm::vec3 target,
            glm::vec3 up);

  /** Temporary function to render a triangle to a viewport
   * @param viewport Viewport to render to
   * @param tri Triangle to render
   * @param lights Lights in the scene
   */
  void renderTriangleToViewport(Viewport *viewport, const Triangle &tri,
                                const std::vector<Light *> lights);

  /** Render the scene to the viewport
   * @param scene The scene to render
   * @returns Viewport width x height pixel array
   */
  void renderToViewport(Viewport *viewport, const SceneNode &scene) override;
};