// Termm--Fall 2023

#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Primitive.hpp"

class Mesh : public Primitive {
public:
  std::vector<Triangle> m_faces;

  Mesh(const std::string &fname);
  Mesh(std::vector<Triangle> faces);

  virtual void print(std::ostream &out) const override;

  virtual glm::vec3 intersectRay(const glm::vec3 &position,
                                 const glm::vec3 &direction) const override;
  virtual glm::vec3 getNormal(const glm::vec3 &intersect) const override;
  virtual Primitive *copy() override;
  virtual void updateBoundingBox() override;
};
