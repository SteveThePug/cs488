#include "Viewport.hpp"
#include <format>
#include <glm/glm.hpp>
#include <iomanip>
#include <vector>
using namespace std;

Viewport::Viewport(int width, int height)
    : width(width), height(height), pixels(width * height) {}

// Set the color of a specific pixel
void Viewport::setPixel(int x, int y, const glm::vec3 &color) {
  if (x >= 0 && x < width && y >= 0 && y < height) {
    pixels[y * width + x] = color;
  }
}

// Get the color of a specific pixel
glm::vec3 Viewport::getPixel(int x, int y) const {
  if (x >= 0 && x < width && y >= 0 && y < height) {
    return pixels[y * width + x];
  }
  return glm::vec3(0.0f); // Return black or an error color if out of bounds
}
glm::vec3 Viewport::operator()(uint x, uint y) const {
  return pixels[y * width + x];
}
glm::vec3 &Viewport::operator()(uint x, uint y) {
  return pixels[y * width + x];
}

// Getters for width and height
int Viewport::getWidth() const { return width; }
int Viewport::getHeight() const { return height; }

std::ostream &operator<<(std::ostream &os, const Viewport &viewport) {
  for (int y = 0; y < viewport.getHeight(); ++y) {
    for (int x = 0; x < viewport.getWidth(); ++x) {
      glm::vec3 color = viewport.getPixel(x, y);
      float length = glm::length(color) / sqrt(3);
      if (length > .8) {
        os << "+";
      } else if (length > 0.6) {
        os << "/";
      } else if (length > 0.4) {
        os << "|";
      } else if (length > 0.2) {
        os << "\\";
      } else {
        os << "-";
      }
    }
    os << '\n';
  }
  return os;
}
