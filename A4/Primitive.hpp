// Termm--Fall 2023

#pragma once

#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include <glm/glm.hpp>

class Ray;

class Primitive {
public:
  // Field variables
  Material *mat;
  // Bounding box
  glm::vec3 bounding_bln;
  glm::vec3 bounding_trf;
  // Constructors
  Primitive();
  Primitive(Material *material);
  virtual ~Primitive();
  // Functional
  bool inBoundingBox(glm::vec3 point) const;
  void updateBoundingBox();
  virtual void print(std::ostream &out) const;
  friend std::ostream &operator<<(std::ostream &out, const Primitive &p);
  virtual glm::vec3 intersectRay(const Ray &ray) const = 0;
  // Setters
  void setMaterial(Material *mat);
  // Getters
  glm::vec3 getBottomLeftNear() const;
  glm::vec3 getTopRightFar() const;
  Material *getMaterial() const;
  virtual glm::vec3 getNormal(const glm::vec3 &intersect) const = 0;
};

class Sphere : public Primitive {
private:
  glm::vec3 m_pos;
  double m_radius;

public:
  // Constructors
  Sphere();
  Sphere(const glm::vec3 &pos, const double &radius);
  Sphere(const glm::vec3 &pos, const double &radius, Material *material);
  virtual ~Sphere();
  // Functional
  glm::vec3 intersectRay(const Ray &ray) const override;
  virtual void print(std::ostream &out) const override;
  void updateBoundingBox();
  // Getters
  glm::vec3 getPosition() const;
  glm::vec3 getNormal(const glm::vec3 &intersect) const override;
  double getRadius() const;
};

class Cube : public Primitive {
private:
  glm::vec3 m_pos;
  double m_radius;

public:
  // Constructors
  Cube();
  Cube(const glm::vec3 &pos, const double &radius);
  Cube(const glm::vec3 &pos, const double &radius, Material *material);
  virtual ~Cube();
  // Functional
  glm::vec3 intersectRay(const Ray &ray) const override;
  void updateBoundingBox();
  // Getters
  glm::vec3 getPosition() const;
  glm::vec3 getNormal(const glm::vec3 &intersect) const override;
  double getRadius() const;
  virtual void print(std::ostream &out) const override;
};

class Triangle : public Primitive {
public:
  Material *m_a, *m_b, *m_c; // Material at each vertex
  glm::vec3 a, b, c;         // Points of vector
  glm::vec3 normal;          // Normal vector
  // Constructors
  Triangle();
  Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);
  Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c,
           Material &mat);
  // Functional
  glm::vec3 intersectRay(const Ray &ray) const override;
  bool pointLiesInPlane(const glm::vec3 &point) const;
  void updateBoundingBox();

  // Convert known point on plane to barycentric coords using possibly computed
  // variables
  glm::vec3 barycentricPoint(const glm::vec3 &point, const glm::vec3 &edge1,
                             const glm::vec3 &edge2, const glm::vec3 &ap) const;
  // Convert known point on plane to barycentric coords
  glm::vec3 barycentricPoint(const glm::vec3 &point) const;

  virtual void print(std::ostream &out) const override;

  // Setters
  void setMaterial(Material *material);
  // Getters
  glm::vec3 getNormal() const;
  glm::vec3 getNormal(const glm::vec3 &intersect) const override;
};
