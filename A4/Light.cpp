// Termm--Fall 2023

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

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

Light *Light::transformedLight(glm::mat4 trans) {
  return new Light(vec3(trans * vec4(position, 1.0)), colour, falloff);
}

std::ostream &operator<<(std::ostream &out, const Light &l) {
  out << "L[" << to_string(l.colour) << ", " << to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0)
      out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
