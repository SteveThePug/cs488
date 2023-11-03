// Termm--Fall 2023

#pragma once

#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include <glm/glm.hpp>

enum class PrimitiveType { Sphere, Cube, NonheirSphere, Triangle };

class Primitive {
public:
  virtual ~Primitive();
  PrimitiveType type;
};

class Sphere : public Primitive {
public:
  Sphere(const glm::vec3 &pos, double radius) : m_pos(pos), m_radius(radius) {}
  Sphere();
  virtual ~Sphere();

  glm::vec3 getPosition() const;
  double getRadius() const;

private:
  glm::vec3 m_pos;
  double m_radius;
};

class Cube : public Primitive {
public:
  Cube(const glm::vec3 &pos, double radius) : m_pos(pos), m_radius(radius) {}
  Cube();
  virtual ~Cube();

  glm::vec3 getPosition() const;
  double getRadius() const;

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3 &pos, double radius)
      : m_pos(pos), m_radius(radius) {}
  virtual ~NonhierSphere();

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3 &pos, double size) : m_pos(pos), m_size(size) {}

  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};

class Triangle : Primitive {
public:
  Material *m_a, *m_b, *m_c; // Material at each vertex
  glm::vec3 a, b, c;         // Points of vector
  glm::vec3 normal;          // Normal vector
  // Constructor
  Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c);

  // Converts point to barycentric coords
  glm::vec3 barycentricPoint(const glm::vec3 &point, const glm::vec3 &edge1,
                             const glm::vec3 &edge2, const glm::vec3 &ap) const;
  // Converts the point to barycentric coords
  glm::vec3 barycentricPoint(const glm::vec3 &point) const;

  /**
   * Compute phong shading based of barycentric_coord
   * @param lights The lights in the scene
   * @param camera The camera observing
   * @param barycentric_coord The position in the triangle
   * @return The shading of the point
   */
  glm::vec3 phongShading(const std::vector<Light *> &lights,
                         const Camera &camera,
                         const glm::vec3 &barycentric_coord) const;

  /**
   * Calculates light intensity at verticies, then linearly interpolates
   * @param lights The lights in the scene
   * @param camera The camera observing
   * @param barycentric_coord The position in the triangle
   * @return The shading of the point
   */
  glm::vec3 gouraudShading(const std::vector<Light> &lights,
                           const Camera &camera,
                           glm::vec3 barycentric_coord) const;

  // Setters
  void setMaterial(Material *material);
  // Getters
  glm::vec3 getNormal() const;
};
