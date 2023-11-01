// Termm-Fall 2023

#pragma once

#include "Material.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <list>
#include <set>
#include <string>

enum class NodeType { SceneNode, GeometryNode, JointNode };

class SceneNode {
public:
  SceneNode(const std::string &name);

  SceneNode(const SceneNode &other);

  virtual ~SceneNode();

  int totalSceneNodes() const;

  const glm::mat4 &get_transform() const;
  const glm::mat4 &get_inverse() const;

  void set_transform(const glm::mat4 &m);

  void add_child(SceneNode *child);

  void remove_child(SceneNode *child);

  //-- Transformations:
  void rotate(char axis, float angle);
  void scale(const glm::vec3 &amount);
  void translate(const glm::vec3 &amount);

  //-- Rendering
  virtual void render(const ShaderProgram &m_shader,
                      const BatchInfoMap &m_batchInfoMap, glm::mat4 m_view,
                      glm::mat4 m_model, const bool picking = false) const;
  // Reset Joints
  virtual void reset();

  // Get Children
  SceneNode *get_child(std::string name);
  SceneNode *get_child(unsigned int id);
  std::set<SceneNode *> get_joints();

  static void updateShaderUniforms(const ShaderProgram &m_shader,
                                   const glm::mat4 &m_view,
                                   const glm::mat4 &m_model,
                                   const glm::vec3 &kd,
                                   const bool picking = false);

  friend std::ostream &operator<<(std::ostream &os, const SceneNode &node);

  bool isSelected;

  // Transformations
  glm::mat4 trans;
  glm::mat4 invtrans;

  std::list<SceneNode *> children;

  NodeType m_nodeType;
  std::string m_name;
  unsigned int m_nodeId;

private:
  // The number of SceneNode instances.
  static unsigned int nodeInstanceCount;
};
