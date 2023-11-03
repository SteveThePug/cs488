#include "Triangle.hpp"
using namespace glm;

Triangle::Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
    : a(a), b(b), c(c) {
  normal = glm::normalize(glm::cross(b - a, c - a));
}

glm::vec3 Triangle::phongShading(const std::vector<Light *> &lights,
                                 const Camera &camera,
                                 const vec3 &barycentric_coord) const {
  if (barycentric_coord == vec3(0)) {
    return vec3(0);
  }
  glm::vec3 color(0.0f); // Resultant color
  glm::vec3 point = a * barycentric_coord.x + b * barycentric_coord.y +
                    c * barycentric_coord.z;
  vec3 camera_pos = camera.getPosition();
  for (const Light *light : lights) {
    glm::vec3 light_pos = light->position;
    // Vector from point on surface to light source and compute falloff
    glm::vec3 toLightVec = light_pos - point;
    float distance = glm::length(toLightVec);
    glm::vec3 toLight = glm::normalize(light_pos - point);
    float falloff = 1.0f / (light->falloff[0] + light->falloff[1] * distance +
                            light->falloff[2] * distance * distance);

    // Vector from point on surface to camera
    glm::vec3 toCamera = glm::normalize(camera_pos - point);

    // Compute diffuse component
    float diff = std::max(glm::dot(normal, toLight), 0.0f) * falloff;

    // Compute specular component
    glm::vec3 reflectDir = glm::reflect(-toLight, normal);
    float spec =
        std::pow(std::max(glm::dot(toCamera, reflectDir), 0.0f), 32) * falloff;

    // Change the light color
    color += (diff * light->colour) + (spec * glm::vec3(1.0f));
  }
  // Clamp the result to [0, 1] range
  color = glm::clamp(color, 0.0f, 1.0f);
  return color;
}

glm::vec3 flatShading(const std::vector<Light> &lights) { return vec3(); }

glm::vec3 gouraudShading(const std::vector<Light> &lights, const Camera &camera,
                         glm::vec3 barycentric_coord) {
  return vec3();
}
glm::vec3 Triangle::getNormal() const { return normal; }
