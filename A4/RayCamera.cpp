#include "RayCamera.hpp"
#include "Ray.hpp"

using namespace glm;

// Constructor
RayCamera::RayCamera(float fov, float aspect, glm::vec3 position,
                     glm::vec3 target, glm::vec3 up)
    : fov(fov), aspect(aspect), Camera(position, target, up) {}

void RayCamera::renderTriangleToViewport(Viewport *viewport,
                                         const Triangle &tri) {
  int width = viewport->getWidth();
  int height = viewport->getHeight();
  float h_fov_radians = glm::radians(fov);
  float v_fov_radians = atan((tan(h_fov_radians) / 2) * aspect) * 2;
  float d_phi = h_fov_radians / width;    // Change in angle for ray
  float d_theta = v_fov_radians / height; // Change in angle for ray
  vec3 cam_to_target = normalize(target - position);
  // d_pi will move in cross(up, Camera to Target) check this for cross
  // Hard bit, calculate change in horozontal vec
  vec3 hor_vec = normalize(cross(up, cam_to_target));
  float alpha = atan(v_fov_radians / 2.0f);
  vec3 d_hor_vec = (alpha * hor_vec) / (width / 2.0f);
  // d_theta will move in cross(^, Camera to Target) check this for cross
  vec3 vert_vec = normalize(cross(hor_vec, cam_to_target));
  float beta = atan(h_fov_radians / 2.0f);
  vec3 d_vert_vec = (alpha * vert_vec) / (height / 2.0f);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      vec3 ver_change = (j - height / 2.0f) * d_vert_vec;
      vec3 hor_change = (i - width / 2.0f) * d_hor_vec;
      vec3 direction = cam_to_target + ver_change + hor_change;
      Ray ray(position, direction);
      vec3 intersect = ray.intersectTriangleBarycentric(tri);
      if (intersect != vec3(0)) {
        viewport->setPixel(i, j, vec3(1, 1, 1));
      } else {
        viewport->setPixel(i, j, vec3(0, 0, 0));
      }
    }
  }
}
