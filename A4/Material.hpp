// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class Material {
private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;

public:
  static Material m_s;

  Material();
  Material(const glm::vec3 &kd, const glm::vec3 &ks, double shininess);
  virtual ~Material();

  /** Get diffusion component
   */
  glm::vec3 getKd() const;

  /** Get specular component
   */
  glm::vec3 getKs() const;

  /** Get shininess
   */
  double getShininess() const;

  // Operations
  Material operator+(const Material &other) const;
  Material operator*(const float &other) const;

  friend std::ostream &operator<<(std::ostream &out, const Primitive &p);
};
