// Termm--Fall 2023
#include "Material.hpp"
#include <glm/ext.hpp>

Material::Material()
    : m_kd(1.0f, 1.0f, 1.0f), m_ks(1.0f, 1.0f, 1.0f), m_shininess(1.0f) {}

Material::Material(const glm::vec3 &kd, const glm::vec3 &ks, double shininess)
    : m_kd(kd), m_ks(ks), m_shininess(shininess) {}

Material::~Material() {}

glm::vec3 Material::getKd() const { return m_kd; }
glm::vec3 Material::getKs() const { return m_ks; }
double Material::getShininess() const { return m_shininess; }

Material Material::operator+(const Material &other) const {
  glm::vec3 f_kd = m_kd + other.getKd();
  glm::vec3 f_ks = m_ks + other.getKs();
  double shininess = m_shininess + other.getShininess();
  return Material(f_kd, f_ks, shininess);
}

Material Material::operator*(const float &other) const {
  glm::vec3 f_kd = m_kd * other;
  glm::vec3 f_ks = m_ks * other;
  double shininess = m_shininess * other;
  return Material(f_kd, f_ks, shininess);
}

std::ostream &operator<<(std::ostream &out, const Material &m) {
  out << "KD[" << glm::to_string(m.m_kd) << "], KS[" << glm::to_string(m.m_ks)
      << "], SH[" << m.m_shininess << "]";
  return out;
}
