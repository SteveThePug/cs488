#pragma once

#include <glm/glm.hpp>
#include <ostream>
#include <vector>

class Viewport {
private:
  int width, height;
  std::vector<glm::vec3> pixels;

public:
  Viewport(int width, int height);

  // Set the color of a specific pixel
  void setPixel(int x, int y, const glm::vec3 &color);

  // Get the color of a specific pixel
  glm::vec3 getPixel(int x, int y) const;
  // Retrieve a particular component from the viewport
  glm::vec3 operator()(uint x, uint y) const;
  // Retrieve address of component from the viewport
  glm::vec3 &operator()(uint x, uint y);

  // Getters for width and height
  int getWidth() const;
  int getHeight() const;

  friend std::ostream &operator<<(std::ostream &os, const Viewport &viewport);
};
