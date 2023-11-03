// Termm--Fall 2023

#pragma once

#include "Camera.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include <glm/glm.hpp>

class Primitive {
public:
  virtual ~Primitive();

  // Shading methods
  /**
   * Interpolates vertex normals across surface
   * @param lights The lights in the scene
   * @param camera The camera observing the point
   * @param barycentric_coord The position in the triangle
   * @return The shading of the point
   */
  glm::vec3 phongShading(const std::vector<Light *> &lights,
                         const Camera &camera,
                         const glm::vec3 &barycentric_coord) const;

  /**
   * Will color vertex points uniformly depending on color and light at each
   * point
   * @param lights The lights in the scene
   * @return The shading of the point
   */
  glm::vec3 flatShading(const std::vector<Light> &lights) const;

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
};

class Sphere : public Primitive {
public:
  Sphere(const glm::vec3 &pos, double radius) : m_pos(pos), m_radius(radius) {}
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

  // Setters
  void setMaterial(Material *material);

  // Getters
  glm::vec3 getNormal() const;
};
