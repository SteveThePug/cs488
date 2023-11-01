#include "Camera.hpp"

class OrthographicCamera : Camera {
private:
  glm::vec3 left_bottom_near;  // Left bottom near vector of viewbox
  glm::vec3 right_top_far;     // Right top far vector of viewbox
  float width, height;         // Width and height of viewport
  glm::mat4 projection_matrix; // Calculated projection matrix
};
