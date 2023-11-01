#include <glm/glm.hpp>
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

  // Getters for width and height
  int getWidth() const;
  int getHeight() const;
};
