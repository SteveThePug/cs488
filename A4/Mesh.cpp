// Termm--Fall 2023

#include <fstream>
#include <iostream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh(const std::string &fname) : m_faces() {
  std::string code;
  double vx, vy, vz;
  size_t s1, s2, s3;

  std::vector<glm::vec3> m_vertices;

  std::ifstream ifs(("Assets/" + fname).c_str());
  while (ifs >> code) {
    if (code == "v") {
      ifs >> vx >> vy >> vz;
      m_vertices.push_back(glm::vec3(vx, vy, vz));
    } else if (code == "f") {
      ifs >> s1 >> s2 >> s3;
      glm::vec3 a = m_vertices[s1 - 1];
      glm::vec3 b = m_vertices[s2 - 1];
      glm::vec3 c = m_vertices[s3 - 1];
      m_faces.push_back(Triangle(a, b, c));
    }
  }
  updateBoundingBox();
}

Mesh::Mesh(std::vector<Triangle> faces) : m_faces(faces) {
  updateBoundingBox();
}

glm::vec3 Mesh::intersectRay(const glm::vec3 &position,
                             const glm::vec3 &direction) const {
  float closestDistance = MAXFLOAT;
  glm::vec3 closestIntersection = glm::vec3(0);
  for (const Triangle &tri : m_faces) {
    glm::vec3 intersect = tri.intersectRay(position, direction);
    if (intersect == glm::vec3(0))
      continue; // No intersection

    float distance = glm::distance(position, intersect);
    if (distance < closestDistance) {
      closestDistance = distance;
      closestIntersection = intersect;
    }
  }
  return closestIntersection;
}
glm::vec3 Mesh::getNormal(const glm::vec3 &intersect) const {
  for (const Triangle &tri : m_faces) {
    if (tri.pointInFace(intersect))
      return tri.normal;
  }
  return glm::vec3(0);
}
void Mesh::updateBoundingBox() {
  glm::vec3 minPoint(std::numeric_limits<float>::max());
  glm::vec3 maxPoint(std::numeric_limits<float>::lowest());
  // Iterate over all triangles to find the mesh's bounding box
  for (Triangle &tri : m_faces) {
    // Ensure the triangle's bounding box is up to date
    tri.updateBoundingBox();

    // Expand the mesh's bounding box to include the triangle's bounding box
    minPoint = glm::min(minPoint, tri.bounding_bln);
    maxPoint = glm::max(maxPoint, tri.bounding_trf);
  }
  bounding_bln = minPoint;
  bounding_trf = maxPoint;
}
Primitive *Mesh::copy() {
  std::vector<Triangle> trans_faces;
  for (Triangle tri : m_faces) {
    trans_faces.push_back(tri);
  }
  return new Mesh(trans_faces);
}

void Mesh::print(std::ostream &out) const {
  out << "Mesh: ";
  out << "Mat[" << *mat << "], ";
  out << "BLN[" << glm::to_string(bounding_bln) << "], ";
  out << "TRF[" << glm::to_string(bounding_trf) << "]";
}
