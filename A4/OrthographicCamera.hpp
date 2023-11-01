#include "Camera.hpp"

class OrthographicCamera : Camera {
private:
  float width, height, depth;
  glm::mat4 projection_matrix; // Calculated projection matrix
public:
  /** Constructor for orthographic camera
   * @param width Width of viewbox (position in middle)
   * @param height Height of viewbox (position in middle)
   * @param depth Depth of viewbox (position at 0)
   */
  OrthographicCamera(float width, float height, float depth, glm::vec3 position,
                     glm::vec3 target, glm::vec3 up);

  /** Set the viewbox
   * @param left_bottom_near left, bottom, near point of viewbox
   * @param right_top_far right, top, far point of viewbox
   */
  void setPerspective(float width, float height, float depth);

  /** Get the projection matrix
   */
  glm::mat4 getProjectionMatrix() override;

  /** Update projection matrix with given viewbox
   */
  void updateProjectionMatrix() override;
};
