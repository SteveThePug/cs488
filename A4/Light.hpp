// Termm--Fall 2023

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a simple point light.
class Light {
public:
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];

  Light();
  Light(glm::vec3 color, glm::vec3 position);
  void setFalloff(glm::vec3 falloff);
  friend std::ostream &operator<<(std::ostream &out, const Light &l);
};
