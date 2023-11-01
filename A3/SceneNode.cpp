// Termm-Fall 2023

#include "SceneNode.hpp"

#include "GeometryNode.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <set>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;

//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string &name)
    : m_name(name), m_nodeType(NodeType::SceneNode), trans(mat4()),
      isSelected(false), m_nodeId(nodeInstanceCount++) {}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode &other)
    : m_nodeType(other.m_nodeType), m_name(other.m_name), trans(other.trans),
      invtrans(other.invtrans) {
  for (SceneNode *child : other.children) {
    this->children.push_front(new SceneNode(*child));
  }
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
  for (SceneNode *child : children) {
    delete child;
  }
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4 &m) {
  trans = m;
  invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4 &SceneNode::get_transform() const { return trans; }

//---------------------------------------------------------------------------------------
const glm::mat4 &SceneNode::get_inverse() const { return invtrans; }

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode *child) { children.push_back(child); }

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode *child) { children.remove(child); }

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
  vec3 rot_axis;

  switch (axis) {
  case 'x':
    rot_axis = vec3(1, 0, 0);
    break;
  case 'y':
    rot_axis = vec3(0, 1, 0);
    break;
  case 'z':
    rot_axis = vec3(0, 0, 1);
    break;
  default:
    break;
  }
  mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
  trans = rot_matrix * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 &amount) {
  trans = glm::scale(amount) * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3 &amount) {
  trans = glm::translate(amount) * trans;
}

//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const { return nodeInstanceCount; }

//---------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const SceneNode &node) {

  // os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____,
  // transform: ____"
  switch (node.m_nodeType) {
  case NodeType::SceneNode:
    os << "SceneNode";
    break;
  case NodeType::GeometryNode:
    os << "GeometryNode";
    break;
  case NodeType::JointNode:
    os << "JointNode";
    break;
  }
  os << ":[";

  os << "name:" << node.m_name << ", ";
  os << "id:" << node.m_nodeId;
  os << "]";

  return os;
}

void SceneNode::updateShaderUniforms(const ShaderProgram &shader,
                                     const glm::mat4 &m_view,
                                     const glm::mat4 &m_model,
                                     const glm::vec3 &kd, const bool picking) {
  shader.enable();
  //-- Set ModelView matrix:
  GLint location = shader.getUniformLocation("ModelView");
  mat4 modelView = m_view * m_model;
  glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
  CHECK_GL_ERRORS;

  //-- Set NormMatrix:
  if (!picking) {
    location = shader.getUniformLocation("NormalMatrix");
    mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
    glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
    CHECK_GL_ERRORS;
  }

  //-- Set Material values:
  location = shader.getUniformLocation("material.kd");
  glUniform3fv(location, 1, value_ptr(kd));
  CHECK_GL_ERRORS;

  shader.disable();
}

void SceneNode::render(const ShaderProgram &m_shader,
                       const BatchInfoMap &m_batchInfoMap, mat4 m_view,
                       mat4 m_model, const bool picking) const {
  // Apply sceneNode transformation
  m_model = m_model * trans;
  for (const SceneNode *node : children) {
    node->render(m_shader, m_batchInfoMap, m_view, m_model, picking);
  }
  return;
}

void SceneNode::reset() {
  for (SceneNode *node : children) {
    node->reset();
  }
  return;
}

SceneNode *SceneNode::get_child(string name) {
  for (SceneNode *node : children) {
    if (node->m_name == name)
      return node;
    SceneNode *child = node->get_child(name);
    if (child)
      return child;
  }
  return NULL;
}

SceneNode *SceneNode::get_child(unsigned int id) {
  for (SceneNode *node : children) {
    if (node->m_nodeId == id)
      return node;
    SceneNode *child = node->get_child(id);
    if (child)
      return child;
  }
  return NULL;
}

set<SceneNode *> SceneNode::get_joints() {
  set<SceneNode *> set1;

  for (SceneNode *node : children) {
    if (node->m_nodeType == NodeType::JointNode) {
      set1.insert(node);
    }
    set<SceneNode *> nodeSet = node->get_joints();
    set1.insert(nodeSet.begin(), nodeSet.end());
  }
  return set1;
}
