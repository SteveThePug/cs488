#include "Camera.hpp"
class PerspectiveCamera : Camera {
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
  PerspectiveCamera(float fov, float aspect, float near_z, float far_z);

  // Getters
  /** Return the projection matrix
   * @return perspective matrix
   */
  glm::mat4 getProjectionMatrix() override;

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
  void updateProjectionMatrix() override;
};
