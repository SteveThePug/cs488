// Termm-Fall 2023

#include "JointNode.hpp"
#include "glm/gtc/matrix_transform.hpp"

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string &name) : SceneNode(name) {
  m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {}
//---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

// Rendering
void JointNode::render(const ShaderProgram &m_shader,
                       const BatchInfoMap &m_batchInfoMap, glm::mat4 m_view,
                       glm::mat4 m_model, const bool picking) const {
  // Apply the joint's transformations
  m_model = m_model * trans;

  // X axis
  float Lx, Ux, x;
  Lx = glm::radians(m_joint_x.min);
  Ux = glm::radians(m_joint_x.max);
  x = glm::radians(m_current_rotation_x);
  if (x < Lx) {
    m_model = glm::rotate(m_model, Lx, glm::vec3(1.0f, 0.0f, 0.0f));
  } else if (x > Ux) {
    m_model = glm::rotate(m_model, Ux, glm::vec3(1.0f, 0.0f, 0.0f));
  } else {
    m_model = glm::rotate(m_model, x, glm::vec3(1.0f, 0.0f, 0.0f));
  }

  // Y axis
  float Ly, Uy, y;
  Ly = glm::radians(m_joint_y.min);
  Uy = glm::radians(m_joint_y.max);
  y = glm::radians(m_current_rotation_y);
  if (y < Ly) {
    m_model = glm::rotate(m_model, Ly, glm::vec3(0.0f, 1.0f, 0.0f));
  } else if (y > Uy) {
    m_model = glm::rotate(m_model, Uy, glm::vec3(0.0f, 1.0f, 0.0f));
  } else {
    m_model = glm::rotate(m_model, y, glm::vec3(0.0f, 1.0f, 0.0f));
  }

  // Render the joint's children
  for (SceneNode *node : children) {
    node->render(m_shader, m_batchInfoMap, m_view, m_model, picking);
  }
  return;
}

void JointNode::reset() {
  m_current_rotation_x = m_joint_x.init;
  m_current_rotation_y = m_joint_y.init;

  for (SceneNode *node : children) {
    node->reset();
  }
  return;
}
