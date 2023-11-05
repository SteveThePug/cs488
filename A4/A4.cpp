// Termm--Fall 2023

#include <glm/ext.hpp>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "Ray.hpp"
#include "RayCamera.hpp"
#include "Scene.hpp"
#include <fstream>
#include <iostream>

using namespace std;
using namespace glm;

void A4_Render(
    // What to render
    SceneNode *root,
    // Image to write to, set to a given width and height
    Image &image,
    // Viewing parameters
    const glm::vec3 &eye, const glm::vec3 &view, const glm::vec3 &up,

    double fovy,
    // Lighting parameters
    const glm::vec3 &ambient, const std::list<Light *> &lights) {

  // Fill in raytracing code here...
  set<SceneNode *> geoms = root->get_geometryNodes();
  list<Primitive *> prims;
  for (SceneNode *geom : geoms) {
    GeometryNode *geomp = static_cast<GeometryNode *>(geom);
    prims.push_back(&geomp->m_primitive);
  }
  std::cout << "F23: Calling A4_Render(\n"
            << "\t" << *root << "\t"
            << "Image(width:" << image.width() << ", height:" << image.height()
            << ")\n"
               "\t"
            << "eye:  " << glm::to_string(eye) << std::endl
            << "\t"
            << "view: " << glm::to_string(view) << std::endl
            << "\t"
            << "up:   " << glm::to_string(up) << std::endl
            << "\t"
            << "fovy: " << fovy << std::endl
            << "\t"
            << "ambient: " << glm::to_string(ambient) << std::endl
            << "\t"
            << "lights{" << std::endl;

  for (const Light *light : lights) {
    std::cout << "\t\t" << *light << std::endl;
  }
  std::cout << "\t}" << std::endl;
  std::cout << "\t"
            << "primitives{" << std::endl;
  for (Primitive *prim : prims) {
    std::cout << "\t\t" << *prim << std::endl;
  }
  std::cout << "\t}" << std::endl;
  std::cout << ")" << std::endl;

  size_t height = image.height();
  size_t width = image.width();

  float aspect = (float)width / height;
  RayCamera camera(fovy, aspect, eye, view, up);

  Viewport viewport(width, height);

  // We need to construct our *prims using *root
  camera.renderPrimitivesToViewport(&viewport, prims, lights);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; ++j) {
      for (int k = 0; k < 3; k++) {
        image(i, j, k) = viewport.getPixel(i, j)[k];
      }
    }
  }
}
