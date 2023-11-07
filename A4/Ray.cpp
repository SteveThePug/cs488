#include "Ray.hpp"
#include <cstddef>
#include <glm/ext.hpp>
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

vec3 Ray::intersectSpherePoint(const Sphere &sphere) const {
  tuple<float, float> t = intersectSphereT(sphere);
  float t0 = get<0>(t);

  if (t0 == 0)
    return vec3(0);
  return getT(t0);
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
    swap(tmin, tmax);

  float tymin = (position.y - radius - origin.y) / direction.y;
  float tymax = (position.y + radius - origin.y) / direction.y;

  if (tymin > tymax)
    swap(tymin, tymax);

  if ((tmin > tymax) || (tymin > tmax))
    return make_tuple(0, 0); // No intersection

  if (tymin > tmin)
    tmin = tymin;

  if (tymax < tmax)
    tmax = tymax;

  float tzmin = (position.z - radius - origin.z) / direction.z;
  float tzmax = (position.z + radius - origin.z) / direction.z;

  if (tzmin > tzmax)
    swap(tzmin, tzmax);

  if ((tmin > tzmax) || (tzmin > tmax))
    return make_tuple(0, 0); // No intersection

  if (tzmin > tmin)
    tmin = tzmin;

  if (tzmax < tmax)
    tmax = tzmax;

  return make_tuple(tmin, tmax); // Intersection at tmin and tmax
}

vec3 Ray::intersectCubePoint(const Cube &cube) const {
  tuple<float, float> intersectT = intersectCubeT(cube);
  float t0 = get<0>(intersectT);
  if (t0 == 0)
    return vec3(0);
  return getT(t0);
}

tuple<vec3, vec3> Ray::intersectCubePoints(const Cube &cube) const {
  tuple<float, float> intersectT = intersectCubeT(cube);
  // If no intersection
  if (intersectT == make_tuple(0, 0))
    return make_tuple(vec3(0), vec3(0));
  // Return intserection points
  return make_tuple(getT(get<0>(intersectT)), getT(get<1>(intersectT)));
}

float Ray::intersectTriangleT(const Triangle &tri) const {
  float offset = -dot(tri.normal, tri.a);
  float t = intersectPlaneT(tri.normal, offset);
  // Ray is parralel with plane
  if (t == 0)
    return 0;
  // Get point on plane of intersection
  vec3 P = getT(t);
  // Calculate edge vectors
  vec3 edge1 = tri.b - tri.a;
  vec3 edge2 = tri.c - tri.b;
  vec3 edge3 = tri.a - tri.c;
  // A,B,C to intersection
  vec3 ap = P - tri.a;
  vec3 bp = P - tri.b;
  vec3 cp = P - tri.c;
  // Calculate cross products
  float test1 = dot(cross(edge1, ap), tri.normal);
  float test2 = dot(cross(edge2, bp), tri.normal);
  float test3 = dot(cross(edge3, cp), tri.normal);
  if (test1 >= 0 && test2 >= 0 && test3 >= 0) {
    return t;
  }
  return 0;
}

vec3 Ray::intersectTrianglePoint(const Triangle &tri) const {
  float offset = -dot(tri.normal, tri.a);
  float t = intersectPlaneT(tri.normal, offset);
  // Ray is parralel with plane
  if (t == 0)
    return vec3(0);
  // Get point on plane of intersection
  vec3 P = getT(t);
  if (!tri.inBoundingBox(P)) {
    return vec3(0);
  }
  // Calculate edge vectors
  vec3 edge1 = tri.b - tri.a;
  vec3 edge2 = tri.c - tri.b;
  vec3 edge3 = tri.a - tri.c;
  // A,B,C to intersection
  vec3 ap = P - tri.a;
  vec3 bp = P - tri.b;
  vec3 cp = P - tri.c;
  // Calculate cross products
  float test1 = dot(cross(edge1, ap), tri.normal);
  float test2 = dot(cross(edge2, bp), tri.normal);
  float test3 = dot(cross(edge3, cp), tri.normal);
  if (test1 >= 0 && test2 >= 0 && test3 >= 0) {
    return P;
  }
  return vec3(0);
}

vec3 Ray::intersectTriangleBarycentric(const Triangle &tri) const {
  float offset = -dot(tri.normal, tri.a);
  float t = intersectPlaneT(tri.normal, offset);
  // Ray is parralel with plane
  if (t == 0)
    return vec3(0);
  // Get point on plane of intersection
  vec3 P = getT(t);
  // Calculate edge vectors
  vec3 edge1 = tri.b - tri.a;
  vec3 edge2 = tri.c - tri.b;
  vec3 edge3 = tri.a - tri.c;
  // A,B,C to intersection
  vec3 ap = P - tri.a;
  vec3 bp = P - tri.b;
  vec3 cp = P - tri.c;
  // Calculate cross products
  float test1 = dot(cross(edge1, ap), tri.normal);
  float test2 = dot(cross(edge2, bp), tri.normal);
  float test3 = dot(cross(edge3, cp), tri.normal);
  if (test1 >= 0 && test2 >= 0 && test3 >= 0) {
    return tri.barycentricPoint(P, edge1, edge2, ap);
  }
  return vec3(0);
}

vec3 Ray::phongShading(const list<Light *> &lights, const Camera &camera,
                       const vec3 &point, const vec3 &normal,
                       const Material &mat) const {
  vec3 color(0.0f);

  vec3 camera_pos = camera.getPosition();
  // Normalise fragment to camera (v)
  vec3 toCamera = normalize(camera_pos - point);

  vec3 kd = mat.getKd();
  vec3 ks = mat.getKs();
  float shininess = mat.getShininess();

  for (const Light *light : lights) {
    vec3 light_pos = light->position;
    vec3 light_color = light->colour;
    vec3 light_falloff = light->falloff;
    // Calculate fragment to light
    vec3 toLightVec = light_pos - point;
    // Distance for falloff
    float distance = length(toLightVec);
    float falloff = 1.0f / (light_falloff[0] + light_falloff[1] * distance +
                            light_falloff[2] * distance * distance);

    // Normalise fragment to light (l)
    vec3 toLight = normalize(toLightVec);
    // n_dot_l
    float n_dot_l = glm::max(dot(normal, toLight), 0.0f);

    // Compute diffuse component
    vec3 diffuse = mat.getKd() * n_dot_l * falloff;

    vec3 specular(0.0f);
    if (n_dot_l > 0) {
      vec3 h = normalize(toLight + toCamera);
      float n_dot_h = glm::max(dot(normal, h), 0.0f);
      specular = ks * pow(n_dot_h, shininess) * falloff;
    }
    color += light_color * (diffuse + specular);
  }
  // Clamp the result to [0, 1] range
  color = clamp(color, 0.0f, 1.0f);
  return color;
}
