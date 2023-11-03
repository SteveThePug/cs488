// Termm--Fall 2023
#include "Primitive.hpp"
#include <glm/ext.hpp>
using namespace glm;
using namespace std;

Primitive::~Primitive() {}

Sphere::~Sphere() {}
glm::vec3 Sphere::getPosition() const { return m_pos; }
double Sphere::getRadius() const { return m_radius; }

Cube::~Cube() {}
glm::vec3 Cube::getPosition() const { return m_pos; }
double Cube::getRadius() const { return m_radius; }

NonhierSphere::~NonhierSphere() {}

NonhierBox::~NonhierBox() {}

Triangle::Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
    : a(a), b(b), c(c) {
  normal = glm::normalize(glm::cross(b - a, c - a));
  m_a = m_b = m_c = &Material::m_s;
}
glm::vec3 Triangle::phongShading(const std::vector<Light *> &lights,
                                 const Camera &camera,
                                 const vec3 &barycentric_coord) const {
  // Check for non - intersection
  if (barycentric_coord == vec3(0)) {
    return vec3(0);
  }

  glm::vec3 color(0.0f); // Resultant color

  // Compute intersection point
  glm::vec3 point = a * barycentric_coord.x + b * barycentric_coord.y +
                    c * barycentric_coord.z;

  // Get material properties
  // Interpolate properties
  Material interpolatedMaterial = *m_a * barycentric_coord.x +
                                  *m_b * barycentric_coord.y +
                                  *m_c * barycentric_coord.z;

  vec3 camera_pos = camera.getPosition();
  for (const Light *light : lights) {
    glm::vec3 light_pos = light->position;
    // Calculate fragment to light
    glm::vec3 toLightVec = light_pos - point;
    // Distance for falloff
    float distance = glm::length(toLightVec);
    float falloff = 1.0f / (light->falloff[0] + light->falloff[1] * distance +
                            light->falloff[2] * distance * distance);

    // Normalise fragment to light
    glm::vec3 toLight = glm::normalize(toLightVec);
    // Normalise fragment to camera
    glm::vec3 toCamera = glm::normalize(camera_pos - point);
    // n_dot_l
    float n_dot_l = std::max(glm::dot(normal, toLight), 0.0f);

    // Compute diffuse component
    glm::vec3 diff = interpolatedMaterial.getKd() * n_dot_l * falloff;

    // Compute specular component (32 in this case would be the shininess)
    glm::vec3 reflectDir = glm::reflect(-toLight, normal);
    float spec_intensity =
        std::pow(std::max(glm::dot(toCamera, reflectDir), 0.0f),
                 interpolatedMaterial.getShininess()) *
        falloff;
    glm::vec3 spec = spec_intensity * interpolatedMaterial.getKs();

    // Change the light color
    color += diff + spec;
  }
  // Clamp the result to [0, 1] range
  color = glm::clamp(color, 0.0f, 1.0f);
  return color;
}
glm::vec3 Triangle::flatShading(const std::vector<Light> &lights) const {
  return vec3();
}
glm::vec3 Triangle::gouraudShading(const std::vector<Light> &lights,
                                   const Camera &camera,
                                   glm::vec3 barycentric_coord) const {
  return vec3();
}
glm::vec3 Triangle::getNormal() const { return normal; }

void Triangle::setMaterial(Material *material) { m_a = m_b = m_c = material; }
