// Termm--Fall 2023
#include "Primitive.hpp"
#include "Ray.hpp"
#include <cstddef>
#include <glm/ext.hpp>
using namespace glm;
using namespace std;

/**
 * Primitive definitions
 */
Primitive::Primitive() : mat(Material::m_s) {}
Primitive::Primitive(Material &material) : mat(material) {}
Primitive::~Primitive() {}
void Primitive::setMaterial(Material &mat) { this->mat = mat; }
Material &Primitive::getMaterial() const { return mat; }
std::ostream &operator<<(std::ostream &out, const Primitive &p) {
  out << "M[" << p.mat << "]";
  return out;
}

/**
 * Sphere definitions
 */
Sphere::Sphere() : Primitive(), m_pos(0.0, 0.0, 0.0), m_radius(0.5) {}
Sphere::Sphere(const glm::vec3 &position, const double &radius)
    : Primitive(), m_pos(position), m_radius(radius) {}
Sphere::Sphere(const glm::vec3 &position, const double &radius,
               Material &material)
    : Primitive(material), m_pos(position), m_radius(radius) {}
Sphere::~Sphere() {}
glm::vec3 Sphere::intersectRay(const Ray &ray) const {
  return ray.intersectSpherePoint(*this);
}
glm::vec3 Sphere::getNormal(const glm::vec3 &intersect) const {
  glm::vec3 normal = intersect - m_pos;
  normal = glm::normalize(normal);
  return normal;
}
glm::vec3 Sphere::getPosition() const { return m_pos; }
double Sphere::getRadius() const { return m_radius; }

/**
 * Cube Definitions
 */
Cube::Cube() : Primitive(), m_pos(0.0f, 0.0f, 0.0f), m_radius(1.0f) {}
Cube::Cube(const glm::vec3 &position, const double &radius)
    : Primitive(), m_pos(position), m_radius(radius) {}
Cube::Cube(const glm::vec3 &position, const double &radius, Material &material)
    : Primitive(material), m_pos(position), m_radius(radius) {}
Cube::~Cube() {}
glm::vec3 Cube::intersectRay(const Ray &ray) const {
  return ray.intersectCubePoint(*this);
}
glm::vec3 Cube::getPosition() const { return m_pos; }
double Cube::getRadius() const { return m_radius; }
glm::vec3 Cube::getNormal(const vec3 &intersect) const {
  glm::vec3 diff = intersect - m_pos;

  glm::vec3 normalized_diff = glm::abs(diff) / static_cast<float>(m_radius);

  glm::vec3 normal(0.0f);

  // Find the maximum component of the normalized difference
  float max_component = std::max(std::max(normalized_diff.x, normalized_diff.y),
                                 normalized_diff.z);

  // Based on the maximum component, decide the normal. It's either pointing in
  // the positive or negative direction of the axis that had the highest
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

/**
 * Triangle Definitions
 */
Triangle::Triangle()
    : Primitive(), a(1.0f, -1.0f, 0.0f), b(0.0f, 1.0f, 0.0f),
      c(-1.0f, -1.0f, 0.0f) {}
Triangle::Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
    : Primitive(), a(a), b(b), c(c) {
  normal = glm::normalize(glm::cross(b - a, c - a));
}
Triangle::Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
                   Material &mat)
    : Triangle(a, b, c) {
  m_a = m_b = m_c = &mat;
}

glm::vec3 Triangle::barycentricPoint(const glm::vec3 &point,
                                     const glm::vec3 &edge1,
                                     const glm::vec3 &edge2,
                                     const glm::vec3 &ap) const {
  // Compute vectors (note that we may have already computed these recently)
  glm::vec3 v0 = edge1, v1 = edge2, v2 = ap;

  // Compute dot products
  float d00 = glm::dot(edge1, edge1);
  float d01 = glm::dot(edge1, edge2);
  float d11 = glm::dot(edge2, edge2);
  float d20 = glm::dot(ap, edge1);
  float d21 = glm::dot(ap, edge2);

  // Compute barycentric coordinates
  float denom = d00 * d11 - d01 * d01;
  float v = (d11 * d20 - d01 * d21) / denom;
  float w = (d00 * d21 - d01 * d20) / denom;
  float u = 1.0f - v - w;

  // Return the barycentric coordinates
  return glm::vec3(u, v, w);
}
bool Triangle::pointLiesInPlane(const glm::vec3 &point) const {
  float offset = -dot(normal, a);
  float planeEq = glm::dot(normal, point) + offset;
  const float epsilon = 1e-6f;
  return std::fabs(planeEq) < epsilon;
}

glm::vec3 Triangle::barycentricPoint(const glm::vec3 &point) const {
  // Compute vectors (note that we may have already computed these recently)
  glm::vec3 v0 = b - a, v1 = c - a, v2 = point - a;
  return barycentricPoint(point, v0, v1, v2);
}
glm::vec3 Triangle::getNormal() const { return normal; }
glm::vec3 Triangle::getNormal(const glm::vec3 &intersect) const {
  return normal;
}
glm::vec3 Triangle::intersectRay(const Ray &ray) const {
  return ray.intersectTrianglePoint(*this);
}
