// Termm--Fall 2023

#pragma once

#include "Material.hpp"
#include <glm/glm.hpp>

class Primitive {
public:
  Material *mat;
  glm::vec3 center;
  glm::vec3 bounding_bln;
  glm::vec3 bounding_trf;

  static Material static_mat;

  Primitive();
  Primitive(Material *material);

  virtual ~Primitive();

  bool pointInBoundingBox(const glm::vec3 &point) const;
  bool rayIntersectBoundingBox(const glm::vec3 &position,
                               const glm::vec3 &direction) const;
  float distanceToPoint(const glm::vec3 &point) const;

  void virtual updateBoundingBox() = 0;

  virtual void print(std::ostream &out) const;
  friend std::ostream &operator<<(std::ostream &out, const Primitive &p);

  virtual glm::vec3 intersectRay(const glm::vec3 &position,
                                 const glm::vec3 &direction) const = 0;
  virtual glm::vec3 getNormal(const glm::vec3 &intersect) const = 0;
  virtual Primitive *copy() = 0;
};

class Sphere : public Primitive {
public:
  glm::vec3 pos;
  double radius;

  Sphere();
  Sphere(const glm::vec3 &pos, const double &radius);
  Sphere(const glm::vec3 &pos, const double &radius, Material *material);
  virtual ~Sphere();

  virtual void print(std::ostream &out) const override;

  virtual glm::vec3 intersectRay(const glm::vec3 &position,
                                 const glm::vec3 &direction) const override;
  virtual glm::vec3 getNormal(const glm::vec3 &intersect) const override;
  Primitive *copy() override;
  virtual void updateBoundingBox() override;
};

class Cube : public Primitive {
public:
  glm::vec3 pos;
  double radius;

  Cube();
  Cube(const glm::vec3 &pos, const double &radius);
  Cube(const glm::vec3 &pos, const double &radius, Material *material);
  virtual ~Cube();

  virtual void print(std::ostream &out) const override;

  virtual glm::vec3 intersectRay(const glm::vec3 &position,
                                 const glm::vec3 &direction) const override;
  glm::vec3 getNormal(const glm::vec3 &intersect) const override;
  Primitive *copy() override;
  virtual void updateBoundingBox() override;
};

class Triangle : public Primitive {
public:
  glm::vec3 a, b, c; // Points of vector
  glm::vec3 normal;  // Normal vector

  Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c);
  Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, Material *mat);

  virtual void print(std::ostream &out) const override;

  bool pointInFace(const glm::vec3 &P) const;
  virtual glm::vec3 intersectRay(const glm::vec3 &position,
                                 const glm::vec3 &direction) const override;
  virtual glm::vec3 getNormal(const glm::vec3 &intersect) const override;
  Primitive *copy() override;
  virtual void updateBoundingBox() override;
  void updateNormal();
};
