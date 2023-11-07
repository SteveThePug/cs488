// Termm--Fall 2023

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <list>
#include <set>
#include <string>

enum class NodeType { SceneNode, GeometryNode, JointNode };
class GeometryNode;

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

  std::set<GeometryNode *> get_geometryNodes();

  //-- Transformations:
  void rotate(char axis, float angle);
  void scale(const glm::vec3 &amount);
  void translate(const glm::vec3 &amount);

  virtual void print(std::ostream &os) const;
  friend std::ostream &operator<<(std::ostream &os, const SceneNode &node);

  // Transformations
  glm::mat4 trans;
  glm::mat4 invtrans;
  bool heirarchal = true;

  std::list<SceneNode *> children;

  NodeType m_nodeType;
  std::string m_name;
  unsigned int m_nodeId;

private:
  // The number of SceneNode instances.
  static unsigned int nodeInstanceCount;
};
