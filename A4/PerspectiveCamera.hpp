#include "Camera.hpp"
class PerspectiveCamera : Camera {
private:
  float fov_x, fov_y;          // X and Y fov
  float near_z, far_z;         // Near and far planes
  float width, height;         // Width and height of viewport
  glm::mat4 projection_matrix; // Calculated projection matrix

public:
};
