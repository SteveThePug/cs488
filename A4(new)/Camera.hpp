#pragma once

#include "Image.hpp"
#include "Light.hpp"
#include "Primitive.hpp"
#include "SceneNode.hpp"
#include "glm/glm.hpp"

class Camera {
public:
  glm::vec3 position;    // Position of camera in world space
  glm::vec3 view;        // Direction camera is looking
  glm::vec3 up;          // Up vector
  glm::mat4 view_matrix; // Calculated view matrix
  float fov, aspect;     // Fov and aspect

  // Constructor
  /** Constructor for the class
   * @param position position of the camera
   * @param target target camera is looking at
   * @param up up vector
   */
  Camera(glm::vec3 position, glm::vec3 viewORtarget, glm::vec3 up);

  /** Constructor for the Camera
   * @param position position of camera
   * @param target target of camera
   * @param up up vector for camera
   * @param fov Fov of camera
   * @param aspect aspect of camera
   * @returns RayCamera the camera
   */
  Camera(glm::vec3 position, glm::vec3 view, glm::vec3 up, float fov,
         float aspect);

  /** Render a set of primitives to a viewport
   * @param img Image to render to
   * @param primitives Primitives to render
   * @param lights Lights in the scene
   */
  void renderSceneToImage(Image &img, SceneNode *scene,
                          const std::list<Light *> &lights, bool cull) const;

  glm::vec3 phongShading(const std::list<Light *> &lights,
                         const glm::vec3 &point, const glm::vec3 &normal,
                         const Material &mat) const;

  // Setters
  /** Set camera position
   * @param position Camera's position
   */
  void setPosition(const glm::vec3 &position);

  /** Set camera orientation
   * @param orientation Camera's orientation
   */
  void setView(const glm::vec3 &view);

  /** Set the up vector
   * @param up The up vector
   */
  void setUp(const glm::vec3 &up);

  // Updates
  /** Generates a projection matrix based on field variables */
  void updateViewMatrix();
  /** Updates the camera view and projection matrix */
  void updateMatricies();
};
