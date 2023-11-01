#include "Scene.hpp"
#include <stdexcept>

Scene::Scene() : root(nullptr), ambient_light(0.0f, 0.0f, 0.0f) {}

void Scene::addCamera(Camera *camera) { cameras.push_back(camera); }

void Scene::addLight(Light *light) { lights.push_back(light); }

void Scene::removeCamera(int index) {
  if (index < 0 || index >= static_cast<int>(cameras.size())) {
    throw std::out_of_range("Camera index is out of range");
  }
  cameras.erase(cameras.begin() + index);
}

void Scene::removeLight(int index) {
  if (index < 0 || index >= static_cast<int>(lights.size())) {
    throw std::out_of_range("Light index is out of range");
  }
  lights.erase(lights.begin() + index);
}

Camera *Scene::getCamera(int index) {
  if (index < 0 || index >= static_cast<int>(cameras.size())) {
    throw std::out_of_range("Camera index is out of range");
  }
  return cameras[index];
}

Light *Scene::getLight(int index) {
  if (index < 0 || index >= static_cast<int>(lights.size())) {
    throw std::out_of_range("Light index is out of range");
  }
  return lights[index];
}

void Scene::setSceneNode(const SceneNode *sceneNode) {
  root = sceneNode; // or root = std::make_unique<SceneNode>(sceneNode); if
                    // using unique_ptr
}

void Scene::setAmbientLight(const glm::vec3 &ambient) {
  ambient_light = ambient;
}

// Using the last available camera, we will render to the viewport.
// Viewport Scene::renderToViewport() {
//
// }
