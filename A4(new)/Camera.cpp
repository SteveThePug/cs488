#include "Camera.hpp"
#include "GeometryNode.hpp"
#include "Mesh.hpp"
#include "glm/gtx/string_cast.hpp"
#include <glm/gtx/transform.hpp> // For glm::lookAt
#include <iostream>

using namespace std;
std::ostream &operator<<(std::ostream &out, const glm::vec3 &v) {
  out << v.x << ", " << v.y << ", " << v.z;
  return out;
}

// Constructor and other necessary methods if needed
Camera::Camera(glm::vec3 position, glm::vec3 view, glm::vec3 up)
    : position(position), up(up) {
  this->view = glm::normalize(view);
  updateMatricies();
}

Camera::Camera(glm::vec3 position, glm::vec3 view, glm::vec3 up, float fov,
               float aspect)
    : Camera(position, view, up) {
  this->fov = fov;
  this->aspect = aspect;
  updateMatricies();
}

// Set camera position
void Camera::setPosition(const glm::vec3 &position) {
  this->position = position;
  updateMatricies();
}

// Set camera target
void Camera::setView(const glm::vec3 &view) {
  this->view = glm::normalize(view);
  updateMatricies();
}

// Set up vector
void Camera::setUp(const glm::vec3 &up) {
  this->up = up;
  updateMatricies();
}

// Update the view matrix
void Camera::updateViewMatrix() {
  this->view_matrix = glm::lookAt(position, position + view, up);
}

// Update all respective camera matricies
void Camera::updateMatricies() { updateViewMatrix(); }

void Camera::renderSceneToImage(Image &img, SceneNode *root,
                                const std::list<Light *> &lights,
                                bool cull) const {

  // Get geometry nodes
  std::set<GeometryNode *> geoms = root->get_geometryNodes();
  // Calculate change in angle
  int width = img.width();
  int height = img.height();
  float aspect = (float)width / height;

  float v_fov_radians = glm::radians(fov);
  float h_fov_radians = 2.0f * atan(tan(v_fov_radians / 2) * aspect);

  glm::vec3 hor = glm::cross(view, up);   // pointing right
  glm::vec3 vert = glm::cross(view, hor); // pointing up

  float h_width = 2.0f * tan(h_fov_radians / 2.0f);
  float v_height = 2.0f * tan(v_fov_radians / 2.0f);

  glm::vec3 d_hor_vec = hor * (h_width / (float)width);
  glm::vec3 d_vert_vec = vert * (v_height / (float)height);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      glm::vec3 hor_change = (float)(i - width / 2.0f) * d_hor_vec;
      glm::vec3 ver_change = (float)(j - height / 2.0f) * d_vert_vec;
      glm::vec3 direction = glm::normalize(ver_change + hor_change + view);

      // Background color
      img(i, j, 0) = 0.0;
      img(i, j, 1) = 0.0;
      img(i, j, 2) = 0.0;

      float min_dist = MAXFLOAT;
      for (GeometryNode *geom : geoms) {
        Primitive &prim = geom->m_primitive;
        // Inverse the ray for model transformations
        glm::vec3 t_pos = glm::vec3(geom->invtrans * glm::vec4(position, 1.0));
        glm::vec3 t_dir = glm::mat3(geom->invtrans) * direction;

        // Check if the ray intersects with the bounding box
        if (!prim.rayIntersectBoundingBox(t_pos, t_dir)) {
          continue;
        }
        // Find model coords intersect
        glm::vec3 intersect = prim.intersectRay(t_pos, t_dir);
        if (intersect == glm::vec3(0))
          continue;
        glm::vec3 normal = prim.getNormal(intersect);
        // Backface culling
        if (cull) {
          if (dot(normal, view) > 1e-6) {
            continue;
          }
        }

        // Convert to world cords
        intersect = glm::vec3(geom->trans * glm::vec4(intersect, 1.0f));
        float distance = glm::distance(intersect, position);
        if (distance > min_dist)
          continue;
        min_dist = distance;

        glm::vec3 color = phongShading(lights, intersect, normal, *prim.mat);
        for (int k = 0; k < 3; k++)
          img(i, j, k) = color[k];
      }
    }
  }
}

glm::vec3 Camera::phongShading(const std::list<Light *> &lights,
                               const glm::vec3 &point, const glm::vec3 &normal,
                               const Material &mat) const {
  glm::vec3 color(0.0f);

  // Normalise fragment to camera (v)
  glm::vec3 toCamera = normalize(position - point);

  glm::vec3 kd = mat.getKd();
  glm::vec3 ks = mat.getKs();
  float shininess = mat.getShininess();

  for (const Light *light : lights) {
    glm::vec3 light_pos = light->position;
    glm::vec3 light_color = light->colour;
    glm::vec3 light_falloff = light->falloff;
    // Calculate fragment to light
    glm::vec3 toLightVec = light_pos - point;
    // Distance for falloff
    float distance = length(toLightVec);
    float falloff = 1.0f / (light_falloff[0] + light_falloff[1] * distance +
                            light_falloff[2] * distance * distance);

    // Normalise fragment to light (l)
    glm::vec3 toLight = normalize(toLightVec);
    // n_dot_l
    float n_dot_l = glm::max(dot(normal, toLight), 0.0f);

    // Compute diffuse component
    glm::vec3 diffuse = mat.getKd() * n_dot_l * falloff;

    glm::vec3 specular(0.0f);
    if (n_dot_l > 0) {
      glm::vec3 h = normalize(toLight + toCamera);
      float n_dot_h = glm::max(dot(normal, h), 0.0f);
      specular = ks * std::pow(n_dot_h, shininess) * falloff;
    }
    color += light_color * (diffuse + specular);
  }
  // Clamp the result to [0, 1] range
  color = clamp(color, 0.0f, 1.0f);
  return color;
}
