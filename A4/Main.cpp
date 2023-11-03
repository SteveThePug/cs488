// Termm--Fall 2023
#include "scene_lua.hpp"

#include "A4.hpp"
#include "PerspectiveCamera.hpp"
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
  int width(10), height(10);
  float fov(90.0f), aspect((float)width / height);
  vector<Light *> lights{};
  vec3 lht_pos(1.0f, 1.0f, 1.0f);
  vec3 lht_col(1.0f, 1.0f, 1.0f);
  Light light(lht_col, lht_pos);
  lights.push_back(&light);
  RayCamera camera(fov, aspect, cam_pos, cam_tar, cam_up);
  Viewport viewport(width, height);

  float time = 0;
  time += 0.001f;
  vec4 a(1.0f, -1.0f, 0.0f, 1.0f), b(-1.0f, -1.0f, 0.0f, 1.0f),
      c(0.0f, 1.0f, 0.0, 1.0f);
  mat4 rotate = glm::rotate(time, vec3(0.0f, 0.0f, 1.0f));
  a = rotate * a;
  b = rotate * b;
  c = rotate * c;
  vec3 ra(a);
  vec3 rb(b);
  vec3 rc(c);
  Triangle triangle = Triangle(ra, rb, rc);
  camera.renderTriangleToViewport(&viewport, triangle, lights);
  cout << viewport;

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
