#include "Viewport.hpp"
#include <glm/glm.hpp>
#include <vector>

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

// Getters for width and height
int Viewport::getWidth() const { return width; }
int Viewport::getHeight() const { return height; }
