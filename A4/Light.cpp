// Termm--Fall 2023
#include "Light.hpp"
#include <glm/ext.hpp>
#include <iostream>

using namespace glm;

Light::Light() : colour(0.0, 0.0, 0.0), position(0.0, 0.0, 0.0) {
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

Light::Light(vec3 color, vec3 position) : colour(color), position(position) {
  Light();
}

std::ostream &operator<<(std::ostream &out, const Light &l) {
  out << "L[" << glm::to_string(l.colour) << ", " << glm::to_string(l.position)
      << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0)
      out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
