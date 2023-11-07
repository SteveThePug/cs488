// Termm--Fall 2023

#include "Primitive.hpp"
#include "Material.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/glm.hpp>
#include <iostream>

/**
 * Primitive definitions
 * -----------------------------------------------------------------------------
 */
Material Primitive::static_mat = Material();

Primitive::Primitive() : mat(&Primitive::static_mat) {}
Primitive::Primitive(Material *material) : mat(material) {}

Primitive::~Primitive() {}

bool Primitive::pointInBoundingBox(const glm::vec3 &point) const {
  return (point.x >= bounding_bln.x && point.x <= bounding_trf.x) &&
         (point.y >= bounding_bln.y && point.y <= bounding_trf.y) &&
         (point.z >= bounding_bln.z && point.z <= bounding_trf.z);
}

bool Primitive::rayIntersectBoundingBox(const glm::vec3 &position,
                                        const glm::vec3 &direction) const {

  glm::vec3 invDir = 1.0f / direction; // element-wise inverse

  glm::vec3 t0 = (bounding_bln - position) * invDir;
  glm::vec3 t1 = (bounding_trf - position) * invDir;

  glm::vec3 tmin = glm::min(t0, t1);
  glm::vec3 tmax = glm::max(t0, t1);

  float tminMax = glm::max(glm::max(tmin.x, tmin.y), tmin.z);
  float tmaxMin = glm::min(glm::min(tmax.x, tmax.y), tmax.z);

  // If tmaxMin < 0, the bounding box is behind the ray
  if (tmaxMin < 0) {
    return false;
  }

  // If tminMax > tmaxMin, there is no intersection
  if (tminMax > tmaxMin) {
    return false;
  }

  // There is an intersection
  return true;
}

float Primitive::distanceToPoint(const glm::vec3 &point) const {
  glm::vec3 center = (bounding_bln + bounding_trf) * 0.5f;
  return glm::distance(center, point);
}

void Primitive::print(std::ostream &out) const {
  out << "Mat[" << *mat << "]";
  return;
}

std::ostream &operator<<(std::ostream &out, const Primitive &p) {
  p.print(out);
  return out;
}

/**
 * Sphere definitions
 * -----------------------------------------------------------------------------
 */
Sphere::Sphere() : Primitive(), pos(0.0, 0.0, 0.0), radius(0.5) {
  updateBoundingBox();
}
Sphere::Sphere(const glm::vec3 &position, const double &radius)
    : Primitive(), pos(position), radius(radius) {
  updateBoundingBox();
}
Sphere::Sphere(const glm::vec3 &position, const double &radius,
               Material *material)
    : Sphere(position, radius) {
  mat = material;
  updateBoundingBox();
}
Sphere::~Sphere() {}

void Sphere::print(std::ostream &out) const {
  out << "Sphere: ";
  out << "Mat[" << *mat << "], ";
  out << "Pos[" << glm::to_string(pos) << "], ";
  out << "Rad[" << radius << "], ";
  out << "BLN[" << glm::to_string(bounding_bln) << "], ";
  out << "TRF[" << glm::to_string(bounding_trf) << "]";
}

glm::vec3 Sphere::intersectRay(const glm::vec3 &position,
                               const glm::vec3 &direction) const {
  glm::vec3 L = pos - position;

  // Quadratic terms
  float a = glm::dot(direction, direction);
  float b = -2.0f * glm::dot(L, direction);
  float c = glm::dot(L, L) - radius * radius;

  // No solution due to negative determinant implies no intersection
  float discriminant = b * b - 4 * a * c;
  if (discriminant < 0) {
    return glm::vec3(0); // No real roots, the ray does not intersect the sphere
  }

  // Calculate the two solutions using the quadratic formula
  float sqrtDiscriminant = sqrt(discriminant);
  float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
  float t1 = (-b + sqrtDiscriminant) / (2.0f * a);

  // Select the smallest positive t
  float t;
  if (t0 > 0 && t1 > 0) {
    t = glm::min(t0, t1);
  } else if (t0 > 0) {
    t = t0;
  } else if (t1 > 0) {
    t = t1;
  } else {
    return glm::vec3(0); // Both t0 and t1 are negative
  }
  glm::vec3 intersection = position + direction * t;
  return intersection;
}
glm::vec3 Sphere::getNormal(const glm::vec3 &intersect) const {
  glm::vec3 normal = intersect - pos;
  normal = glm::normalize(normal);
  return normal;
}
Primitive *Sphere::copy() { return new Sphere(pos, radius, mat); }

void Sphere::updateBoundingBox() {
  glm::vec3 radiusVec(radius);
  bounding_bln = pos - radiusVec;
  bounding_trf = pos + radiusVec;
}

/**
 * Cube definitions   ---------------------------------------------
 */
Cube::Cube() : Primitive(), pos(0.0f, 0.0f, 0.0f), radius(1.0f) {
  updateBoundingBox();
}
Cube::Cube(const glm::vec3 &pos, const double &radius)
    : pos(pos), radius(radius) {
  updateBoundingBox();
}
Cube::Cube(const glm::vec3 &pos, const double &radius, Material *material)
    : Cube(pos, radius) {
  updateBoundingBox();
  mat = material;
}
Cube::~Cube() {}

void Cube::print(std::ostream &out) const {
  out << "Cube: ";
  out << "Mat[" << *mat << "], ";
  out << "Pos[" << glm::to_string(pos) << "], ";
  out << "Rad[" << radius << "]";
}

glm::vec3 Cube::intersectRay(const glm::vec3 &position,
                             const glm::vec3 &direction) const {
  glm::vec3 invDir = 1.0f / direction;
  glm::vec3 t0 = ((pos - glm::vec3(radius)) - position) * invDir;
  glm::vec3 t1 = ((pos + glm::vec3(radius)) - position) * invDir;

  glm::vec3 tmin = glm::min(t0, t1);
  glm::vec3 tmax = glm::max(t0, t1);

  float tMin = std::max(tmin.x, tmin.y);
  float tMax = std::min(tmax.x, tmax.y);
  tMin = std::max(tMin, tmin.z);
  tMax = std::min(tMax, tmax.z);

  // If tMax < 0, the cube is behind the ray
  if (tMax < 0) {
    glm::vec3(0);
  }

  // If tMin > tMax, there is no intersection
  if (tMin > tMax) {
    glm::vec3(0);
  }

  if (tMin < 0) {
    return position + tMax * direction; // Use tMax to get the exit point
  }
  return position + tMin * direction;
}

glm::vec3 Cube::getNormal(const glm::vec3 &intersect) const {
  glm::vec3 diff = intersect - pos;
  glm::vec3 normalized_diff = glm::abs(diff) / static_cast<float>(radius);
  glm::vec3 normal(0.0f);

  // Find the maximum component of the normalized difference
  float max_component = std::max(std::max(normalized_diff.x, normalized_diff.y),
                                 normalized_diff.z);

  // Based on the maximum component, decide the normal. It's either pointing
  // in the positive or negative direction of the axis that had the highest
  // normalized difference.
  if (max_component == normalized_diff.x) {
    normal.x = (diff.x > 0) ? 1.0f : -1.0f;
  } else if (max_component == normalized_diff.y) {
    normal.y = (diff.y > 0) ? 1.0f : -1.0f;
  } else {
    normal.z = (diff.z > 0) ? 1.0f : -1.0f;
  }

  return normal;
}
Primitive *Cube::copy() { return new Cube(pos, radius, mat); }
void Cube::updateBoundingBox() {
  glm::vec3 radiusVec(radius);
  bounding_bln = pos - 1.01f * radiusVec;
  bounding_trf = pos + 1.01f * radiusVec;
}

Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) : a(a), b(b), c(c) {
  updateNormal();
  updateBoundingBox();
}
Triangle::Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Material *material)
    : a(a), b(b), c(c) {
  mat = material;
  updateNormal();
  updateBoundingBox();
}

void Triangle::print(std::ostream &out) const {
  out << "Triangle: ";
  out << "Mat[" << *mat << "], ";
  out << "A[" << glm::to_string(a) << "], ";
  out << "B[" << glm::to_string(b) << "], ";
  out << "C[" << glm::to_string(c) << "], ";
  out << "BLN[" << glm::to_string(bounding_bln) << "], ";
  out << "TRF[" << glm::to_string(bounding_trf) << "]";
}

bool Triangle::pointInFace(const glm::vec3 &P) const {
  glm::vec3 edge1 = b - a;
  glm::vec3 edge2 = c - b;
  glm::vec3 edge3 = a - c;

  glm::vec3 ap = P - a;
  glm::vec3 bp = P - b;
  glm::vec3 cp = P - c;

  glm::vec3 cross1 = cross(edge1, ap);
  glm::vec3 cross2 = cross(edge2, bp);
  glm::vec3 cross3 = cross(edge3, cp);

  if (dot(cross1, normal) >= 0 && dot(cross2, normal) >= 0 &&
      dot(cross3, normal) >= 0) {
    return true; // The point is inside the triangle
  }
  return false;
}

glm::vec3 Triangle::intersectRay(const glm::vec3 &origin,
                                 const glm::vec3 &direction) const {
  float denominator = dot(normal, direction);
  // If the denominator is close to zero, the ray is parallel to the triangle
  if (abs(denominator) < 1e-6) {
    return glm::vec3(0);
  }

  // The intersection point with the plane of the triangle
  float t = dot(normal, a - origin) / denominator;

  // If t is less than or equal to zero, the intersection is behind the ray's
  // origin
  if (t <= 0)
    return glm::vec3(0);

  // Calculate the exact intersection point
  glm::vec3 P = origin + t * direction;

  if (pointInFace(P))
    return P;

  return glm::vec3(0);
}
glm::vec3 Triangle::getNormal(const glm::vec3 &intersect) const {
  return normal;
}
Primitive *Triangle::copy() { return new Triangle(a, b, c, mat); }
void Triangle::updateNormal() {
  normal = glm::normalize(glm::cross(b - a, c - a));
}
void Triangle::updateBoundingBox() {
  glm::vec3 minPoint = a;
  glm::vec3 maxPoint = a;
  minPoint = glm::min(minPoint, b);
  maxPoint = glm::max(maxPoint, b);
  minPoint = glm::min(minPoint, c);
  maxPoint = glm::max(maxPoint, c);
  bounding_bln = minPoint;
  bounding_trf = maxPoint;
}
