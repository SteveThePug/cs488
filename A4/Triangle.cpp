#include "Triangle.hpp"
using namespace glm;

Triangle::Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
    : a(a), b(b), c(c) {
  normal = glm::normalize(glm::cross(b - a, c - a));
}

glm::vec3 phongShading(const std::vector<Light> &lights, const Camera &camera,
                       glm::vec3 barycentric_coord) {
  return vec3();
}

glm::vec3 flatShading(const std::vector<Light> &lights) { return vec3(); }

glm::vec3 gouraudShading(const std::vector<Light> &lights, const Camera &camera,
                         glm::vec3 barycentric_coord) {
  return vec3();
}
glm::vec3 Triangle::getNormal() const { return normal; }
