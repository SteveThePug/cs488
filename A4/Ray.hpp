#pragma once

#include "Triangle.hpp"
#include <glm/glm.hpp>
#include <optional>
#include <tuple>

/**
 * Class representing a Ray in 3D space
 *
 * Defined by the rays start point, and by the direction it will travel
 */
class Ray {
private:
  glm::vec3 origin;
  // Origin of ray
  glm::vec3 direction;
  // Directioin of travel for ray

public:
  Ray(const Ray &) = default;
  Ray(Ray &&) = default;
  Ray &operator=(const Ray &) = default;
  Ray &operator=(Ray &&) = default;
  /**
   * construct a ray with given origin and direction
   * @param origin The start point of the ray
   * @param direction The direction of the ray (will be normalised)
   */
  Ray(const glm::vec3 &origin, const glm::vec3 &direction);

  /**
   * Computes the point the ray is at at time t
   * @param t The parameter to move along the ray
   * @return The point in 3D space at t
   */
  glm::vec3 getT(float t) const;

  /**
   * Computes where a Ray intersects a Plane
   * @param normal The normal of the Plane
   * @param offset The offset of the Plane
   * @return The value of t at the intersection, 0 if no intersection
   */
  float intersectPlaneT(glm::vec3 normal, float offset) const;

  /**
   * Computes where a Ray intersects a Plane
   * @param normal The normal of the Plane
   * @param offset The offset of the Plane
   * @return The point at which the Ray intersects the Plane, vec3(0) for no
   *intersection
   */
  glm::vec3 intersectPlanePoint(glm::vec3 normal, float offset) const;

  /**
   * Computes where a Ray intersects a Sphere
   * @param position The position of the Sphere
   * @param radius The radius of the Sphere
   * @return The value of t at the intersection, 0 for no intersection
   */
  std::tuple<float, float> intersectSphereT(glm::vec3 position,
                                            float radius) const;

  /**
   * Computes where a Ray intersects a Sphere
   * @param position The position of the Sphere
   * @param radius The radius of the Sphere
   * @return The point of the intersection, vec3(0) for no intersection
   */
  std::tuple<glm::vec3, glm::vec3> intersectSpherePoints(glm::vec3 position,
                                                         float radius) const;

  /**
   * Ray Triangle intesection
   * @param Ray Incoming ray
   * @return Barycentric cord of the collision
   * @note Returns vec3(0) if there is no intersection
   */
  glm::vec3 intersectTriangleBarycentric(const Triangle &tri) const;
};
