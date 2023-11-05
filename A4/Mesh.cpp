// Termm--Fall 2023

#include <fstream>
#include <iostream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh(const std::string &fname) : m_vertices(), m_faces() {
  std::string code;
  double vx, vy, vz;
  size_t s1, s2, s3;

  std::ifstream ifs(("Assets/" + fname).c_str());
  while (ifs >> code) {
    if (code == "v") {
      ifs >> vx >> vy >> vz;
      m_vertices.push_back(glm::vec3(vx, vy, vz));
    } else if (code == "f") {
      ifs >> s1 >> s2 >> s3;
      glm::vec3 *a = &m_vertices[s1 - 1];
      glm::vec3 *b = &m_vertices[s2 - 1];
      glm::vec3 *c = &m_vertices[s3 - 1];
      m_faces.push_back(Triangle(*a, *b, *c));
    }
  }
}

void Mesh::print(std::ostream &out) const {
  out << "Mesh: "
      << "M[" << *mat << "]";
}

std::ostream &operator<<(std::ostream &out, const Mesh &mesh) {
  out << "mesh {";
  for (size_t idx = 0; idx < mesh.m_vertices.size(); ++idx) {
    glm::vec3 v = mesh.m_vertices[idx];
    out << glm::to_string(v);
  }
  out << "}";
  return out;
}

glm::vec3 Mesh::intersectRay(const Ray &ray) const {
  glm::vec3 intersect = glm::vec3(0);

  for (const Triangle &tri : m_faces) {
    intersect = tri.intersectRay(ray);

    if (intersect != glm::vec3(0)) {
      return intersect;
    }
  }

  return glm::vec3(0);
}

glm::vec3 Mesh::getNormal(const glm::vec3 &intersect) const {
  for (const Triangle &tri : m_faces) {
    if (tri.pointLiesInPlane(intersect))
      return tri.normal;
  }
  return glm::vec3(0);
}
