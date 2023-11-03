#include "Ray.hpp"
#include <cstddef>
using namespace glm;
using namespace std;

Ray::Ray(const vec3 &origin, const vec3 &direction) : origin(origin) {
  this->direction = normalize(direction);
}

vec3 Ray::getT(float t) const { return origin + t * direction; }

float Ray::intersectPlaneT(vec3 normal, float offset) const {
  float denominator = dot(normal, direction);
  // Then no intersection point (Parallel)
  if (abs(denominator) < 1e-6) {
    return 0;
  }
  // Intersection formula
  float t = -(dot(normal, origin) + offset) / denominator;
  return t;
}

vec3 Ray::intersectPlanePoint(vec3 normal, float offset) const {
  float t = intersectPlaneT(normal, offset);
  if (t < 0)
    return vec3(0, 0, 0);
  return getT(t);
}

tuple<float, float> Ray::intersectSphereT(const Sphere &sphere) const {
  vec3 position = sphere.getPosition();
  double radius = sphere.getRadius();

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

tuple<vec3, vec3> Ray::intersectSpherePoints(const Sphere &sphere) const {
  tuple<float, float> t = intersectSphereT(sphere);
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

tuple<float, float> Ray::intersectCubeT(const Cube &cube) const {
  vec3 position = cube.getPosition();
  double radius = cube.getRadius();
  float tmin = (position.x - radius - origin.x) / direction.x;
  float tmax = (position.x + radius - origin.x) / direction.x;

  if (tmin > tmax)
    std::swap(tmin, tmax);

  float tymin = (position.y - radius - origin.y) / direction.y;
  float tymax = (position.y + radius - origin.y) / direction.y;

  if (tymin > tymax)
    std::swap(tymin, tymax);

  if ((tmin > tymax) || (tymin > tmax))
    return make_tuple(0, 0); // No intersection

  if (tymin > tmin)
    tmin = tymin;

  if (tymax < tmax)
    tmax = tymax;

  float tzmin = (position.z - radius - origin.z) / direction.z;
  float tzmax = (position.z + radius - origin.z) / direction.z;

  if (tzmin > tzmax)
    std::swap(tzmin, tzmax);

  if ((tmin > tzmax) || (tzmin > tmax))
    return make_tuple(0, 0); // No intersection

  if (tzmin > tmin)
    tmin = tzmin;

  if (tzmax < tmax)
    tmax = tzmax;

  return make_tuple(tmin, tmax); // Intersection at tmin and tmax
}

tuple<vec3, vec3> Ray::intersectCubePoints(vec3 position, float radius) const {
  tuple<float, float> intersectT = intersectCubeT(position, radius);
  // If no intersection
  if (intersectT == make_tuple(0, 0))
    return make_tuple(vec3(0), vec3(0));
  // Return intserection points
  return make_tuple(getT(get<0>(intersectT)), getT(get<1>(intersectT)));
}

float Ray::intersectTriangleT(const Triangle &tri) const {

  float offset = -dot(tri.normal, tri.a);
  float t = intersectPlaneT(tri.normal, offset);

  if (t == 0)
    return 0;

  // Intersection point
  glm::vec3 P = getT(t);
}

vec3 Ray::intersectTrianglePoint(const Triangle &tri) const {
  float t = intersectTriangleT(tri);
  if (t == 0)
    return vec3(0);
}

vec3 Ray::intersectTriangleBarycentric(const Triangle &tri) const {
  float offset = -dot(tri.normal, tri.a);
  float t = intersectPlaneT(tri.normal, offset);

  if (t == 0)
    return vec3(0);

  // Intersection point
  glm::vec3 P = getT(t);
  // Edge vectors
  glm::vec3 edge1 = tri.b - tri.a;
  glm::vec3 edge2 = tri.c - tri.a;
  // A to intersection
  glm::vec3 vp = P - tri.a;
  // Dot products
  float dot00 = dot(edge1, edge1);
  float dot01 = dot(edge1, edge2);
  float dot02 = dot(edge1, vp);
  float dot11 = dot(edge2, edge2);
  float dot12 = dot(edge2, vp);
  // Compute barycentric coords
  float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
  float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
  float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
  float w = 1 - u - v;
  // Check we lie in the triangle
  if ((u >= 0) && (v >= 0) && (u + v <= 1)) {
    return vec3(u, v, w); // The intersection is within the triangle
  }
  // No collision
  return vec3(0);
}
