#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Ray.hpp"
#include <glm/glm.hpp>
#include <optional>
#include <vector>
class Triangle {
private:
  glm::vec3 a, b, c;                // Points of vector
  glm::vec3 normal;                 // Normal vector
  glm::vec3 norm_a, norm_b, norm_c; // Normals at points
  Material mat_a, mat_b, mat_c;     // Materials at points

public:
  // Constructor
  Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);

  // Method to calculate and return the normal of the triangle
  glm::vec3 calculateNormal() const;

  /**
   * Ray Triangle intesection
   * @param Ray Incoming ray
   * @return The value of the parameter t at intersection
   * @note Returns 0 if there is no intersection
   */
  float intersectT(const Ray &ray) const;
  /**
   * Ray Triangle intesection
   * @param Ray Incoming ray
   * @return The vec3 point of intersection
   * @note Returns vec3(0) if there is no intersection
   */
  glm::vec3 intersectPoint(const Ray &ray) const;
  /**
   * Ray Triangle intesection
   * @param Ray Incoming ray
   * @return Barycentric cord of the collision
   * @note Returns vec3(0) if there is no intersection
   */
  glm::vec3 intersectBarycentric(const Ray &ray) const;

  // Shading methods

  /**
   * Interpolates vertex normals across surface
   * @param lights The lights in the scene
   * @param camera The camera observing the point
   * @param barycentric_coord The position in the triangle
   * @return The shading of the point
   */
  glm::vec3 phongShading(const std::vector<Light> &lights, const Camera &camera,
                         glm::vec3 barycentric_coord) const;

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

  // Additional utility methods as needed
};
