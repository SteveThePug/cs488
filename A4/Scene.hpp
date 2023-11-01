#include "Camera.hpp"
#include "Light.hpp"
#include "SceneNode.hpp"
#include <vector>

/**
 * Class representing a Scene in a ray tracer or graphics renderer.
 *
 * A Scene is composed of various Cameras, Lights, and a hierarchical structure
 *of SceneNodes. This class provides methods to manage these components.
 */
class Scene {
private:
  std::vector<Camera> cameras; // Collection of Cameras in the Scene
  std::vector<Light> lights;   // Collection of Lights in the Scene
  SceneNode *root;             // Root of the SceneNode hierarchy
  glm::vec3 ambient_light;     // Ambient light in the Scene

public:
  /**
   * Default constructor for creating a Scene.
   */
  Scene();

  /**
   * Adds a Camera to the Scene.
   * @param camera The Camera to be added.
   */
  void addCamera(Camera camera);

  /**
   * Adds a Light to the Scene.
   * @param light The Light to be added.
   */
  void addLight(Light light);

  /**
   * Removes a Camera from the Scene by index.
   * @param index The index of the Camera to remove.
   * @throws std::out_of_range if index out of range
   */
  void removeCamera(int index);

  /**
   * Removes a Light from the Scene by index.
   * @param index The index of the Light to remove.
   * @throws std::out_of_range if index out of range
   */
  void removeLight(int index);

  /**
   * Retrieves a Camera from the Scene by index.
   * @param index The index of the Camera to retrieve.
   * @return The requested Camera.
   * @throws std::out_of_range if index out of range
   */
  Camera getCamera(int index);

  /**
   * Retrieves a Light from the Scene by index.
   * @param index The index of the Light to retrieve.
   * @return The requested Light.
   * @throws std::out_of_range if index out of range
   */
  Light getLight(int index);

  /**
   * Sets the root SceneNode for the Scene.
   * @param sceneNode The root SceneNode to be set.
   */
  void setSceneNode(const SceneNode &sceneNode);

  /**
   * Sets the ambient light in a scene
   * @param ambient The vec3 color of the ambient light
   */
};
