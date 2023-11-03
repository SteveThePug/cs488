// Termm--Fall 2023
#include "Primitive.hpp"
#include "scene_lua.hpp"

#include "A4.hpp"
#include "Image.hpp"
#include "Ray.hpp"
#include "RayCamera.hpp"
#include "Scene.hpp"
#include <fstream>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace glm;
using namespace std;

int main(int argc, char **argv) {
  // Generate the camera
  float alpha = 1 / atan(3.141f / 4) + 1;
  vec3 cam_pos(0.0f, 0.0f, alpha);
  vec3 cam_tar(0.0f, 0.0f, 0.0f);
  vec3 cam_up(0.0f, 1.0f, 0.0f);
  int width(1000), height(700);
  float fov(90.0f), aspect((float)width / height);
  vector<Light *> lights{};
  vec3 lht_pos(1.0f, 1.0f, 1.0f);
  vec3 lht_col(1.0f, 1.0f, 1.0f);
  Light light(lht_col, lht_pos);
  vec3 falloff(1.0f, 0.0f, 0.0f);
  light.setFalloff(falloff);
  lights.push_back(&light);
  RayCamera camera(fov, aspect, cam_pos, cam_tar, cam_up);
  Viewport viewport(width, height);
  vec3 a(1.0f, -1.0f, 0.0f), b(0.0f, 1.0f, 0.0f), c(-1.0f, -1.0f, 0.0f);
  Material m_a(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f);
  Material m_b(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
  Material m_c(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
  Triangle triangle = Triangle(a, b, c);
  triangle.m_a = &m_a;
  triangle.m_b = &m_b;
  triangle.m_c = &m_c;
  camera.renderTriangleToViewport(&viewport, triangle, lights);
  Image img(width, height);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; ++j) {
      for (int k = 0; k < 3; k++) {
        img(i, j, k) = viewport(i, j)[k];
      }
    }
  }
  img.savePng("test2.png");

  std::string filename = "simple.lua";

  if (argc >= 2) {
    filename = argv[1];
  }

  if (!run_lua(filename)) {
    std::cerr << "Could not open " << filename
              << ". Try running the executable from inside of"
              << " the Assets/ directory" << std::endl;
    return 1;
  }
}
