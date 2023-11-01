#include "Ray.hpp"
#include <cstddef>
using namespace glm;
using namespace std;

Ray::Ray(const vec3 &origin, const vec3 &direction)
    : origin(origin), direction(direction) {}

vec3 Ray::getT(float t) { return origin + t * direction; }

float Ray::intersectPlaneT(vec3 normal, float offset) {
  float denominator = dot(normal, direction);
  // Then no intersection point (Parallel)
  if (abs(denominator) < 1e-6) {
    return 0;
  }
  // Intersection formula
  float t = -(dot(normal, origin) + offset) / denominator;
  return t;
}

vec3 Ray::intersectPlanePoint(vec3 normal, float offset) {
  float t = intersectPlaneT(normal, offset);
  if (t < 0)
    return vec3(0, 0, 0);
  return getT(t);
}

tuple<float, float> Ray::intersectSphereT(vec3 position, float radius) {
  // Quadratic terms
  vec3 oc = origin - position;
  float a = dot(direction, direction);
  float b = 2.0f * dot(oc, direction);
  float c = dot(oc, oc) - radius * radius;
  // No solution due to 0 determinant
  float discriminant = b * b - 4 * a * c;
  if (discriminant < 0) {
    return make_tuple(0, 0);
  }
  // Intersection t0, t1
  float sqrtDiscriminant = sqrt(discriminant);
  float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
  float t1 = (-b + sqrtDiscriminant) / (2.0f * a);
  // If solutions are negative set to 0
  if (t0 < 0)
    t0 = 0;
  if (t1 < 0)
    t1 = 0;
  // Fin
  return make_tuple(t0, t1);
}

tuple<vec3, vec3> Ray::intersectSpherePoints(vec3 position, float radius) {
  tuple<float, float> t = intersectSphereT(position, radius);
  float t0 = get<0>(t);
  float t1 = get<1>(t);
  vec3 p0;
  vec3 p1;

  if (t0 == 0)
    p0 = vec3(0);
  else
    p0 = getT(t0);

  if (t1 == 0)
    p1 = vec3(0);
  else
    p1 = getT(t1);

  // Intersection at t0 t1
  return make_tuple(p0, p1);
}
