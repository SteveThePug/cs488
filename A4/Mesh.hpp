// Termm--Fall 2023

#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Primitive.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
// #define RENDER_BOUNDING_VOLUMES

// A polygonal mesh.
class Mesh : public Primitive {
private:
  std::vector<glm::vec3> m_vertices;
  std::vector<Triangle> m_faces;

public:
  Mesh(const std::string &fname);
  glm::vec3 intersectRay(const Ray &ray) const override;
  glm::vec3 getNormal(const glm::vec3 &intersect) const override;
  friend std::ostream &operator<<(std::ostream &out, const Mesh &mesh);
};
