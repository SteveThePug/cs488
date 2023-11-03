// Termm--Fall 2023
#include "Primitive.hpp"
#include <glm/ext.hpp>
using namespace glm;
using namespace std;

Primitive::~Primitive() {}

Sphere::Sphere() {}
Sphere::~Sphere() {}
glm::vec3 Sphere::getPosition() const { return m_pos; }
double Sphere::getRadius() const { return m_radius; }

Cube::Cube() {}
Cube::~Cube() {}
glm::vec3 Cube::getPosition() const { return m_pos; }
double Cube::getRadius() const { return m_radius; }

NonhierSphere::~NonhierSphere() {}

NonhierBox::~NonhierBox() {}

Triangle::Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
    : a(a), b(b), c(c) {
  normal = glm::normalize(glm::cross(b - a, c - a));
  m_a = m_b = m_c = &Material::m_s;
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

glm::vec3 Triangle::barycentricPoint(const glm::vec3 &point) const {
  // Compute vectors (note that we may have already computed these recently)
  glm::vec3 v0 = b - a, v1 = c - a, v2 = point - a;
  return barycentricPoint(point, v0, v1, v2);
}

// Gets triangle normal
glm::vec3 Triangle::getNormal() const { return normal; }
// Sets material of triangle uniformly
void Triangle::setMaterial(Material *material) { m_a = m_b = m_c = material; }
