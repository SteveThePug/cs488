#include "Camera.hpp"

class RayCamera : public Camera {
private:
public:
  /** Render the scene to the viewport
   * @param scene The scene to render
   * @returns Viewport width x height pixel array
   */
  Viewport renderToViewport(SceneNode scene) override;
};
