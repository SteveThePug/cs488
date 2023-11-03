#pragma once

#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include <glm/glm.hpp>
#include <optional>
#include <vector>

class Triangle {
private:
  Material mat_a, mat_b, mat_c; // Materials at points

public:
  glm::vec3 a, b, c; // Points of vector
  glm::vec3 normal;  // Normal vector
  // Constructor
  Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);

  // Shading methods
  /**
   * Interpolates vertex normals across surface
   * @param lights The lights in the scene
   * @param camera The camera observing the point
   * @param barycentric_coord The position in the triangle
   * @return The shading of the point
   */
  glm::vec3 phongShading(const std::vector<Light *> &lights,
                         const Camera &camera,
                         const glm::vec3 &barycentric_coord) const;

  /**
   * Will color vertex points uniformly depending on color and light at each
   * point
   * @param lights The lights in the scene
   * @return The shading of the point
   */
  glm::vec3 flatShading(const std::vector<Light> &lights) const;

  /**
   * Calculates light intensity at verticies, then linearly interpolates
   * @param lights The lights in the scene
   * @param camera The camera observing
   * @param barycentric_coord The position in the triangle
   * @return The shading of the point
   */
  glm::vec3 gouraudShading(const std::vector<Light> &lights,
                           const Camera &camera,
                           glm::vec3 barycentric_coord) const;

  // Getters
  glm::vec3 getNormal() const;
};
