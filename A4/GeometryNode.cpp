// Termm--Fall 2023

#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(const std::string &name, Primitive &prim)
    : SceneNode(name), m_primitive(prim) {
  m_nodeType = NodeType::GeometryNode;
  setMaterial(&Primitive::static_mat);
}
GeometryNode::GeometryNode(const std::string &name, Primitive &prim,
                           Material *mat)
    : SceneNode(name), m_primitive(prim) {
  m_nodeType = NodeType::GeometryNode;
  setMaterial(mat);
}

void GeometryNode::setMaterial(Material *mat) { m_primitive.mat = mat; }

void GeometryNode::print(std::ostream &os) const {
  os << "GeometryNode";
  os << ":[";
  os << "name:" << m_name << ", ";
  os << "id:" << m_nodeId;
  os << "]\n";
  os << m_primitive;
}
