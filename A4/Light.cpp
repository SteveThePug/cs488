// Termm--Fall 2023
#include "Light.hpp"
#include <glm/ext.hpp>
#include <iostream>

using namespace glm;

Light::Light()
    : colour(0.0, 0.0, 0.0), position(0.0, 0.0, 0.0),
      falloff(1.0f, 0.0f, 0.0f) {}

Light::Light(vec3 position)
    : position(position), colour(0.0, 0.0, 0.0), falloff(1.0f, 0.0f, 0.0f) {}

Light::Light(vec3 position, vec3 color)
    : position(position), colour(color), falloff(1.0f, 0.0f, 0.0f) {
  Light();
}
Light::Light(vec3 position, vec3 color, vec3 falloff)
    : position(position), colour(color), falloff(falloff) {
  Light();
}

void Light::setFalloff(vec3 falloff) { this->falloff = falloff; }

std::ostream &operator<<(std::ostream &out, const Light &l) {
  out << "L[" << glm::to_string(l.colour) << ", " << glm::to_string(l.position)
      << ", " << glm::to_string(l.falloff) << "]";
  return out;
}
