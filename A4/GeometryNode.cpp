// Termm--Fall 2023

#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(const std::string &name, Primitive &prim)
    : SceneNode(name), m_primitive(prim) {
  m_nodeType = NodeType::GeometryNode;
  setMaterial(&Material::m_s);
}
GeometryNode::GeometryNode(const std::string &name, Primitive &prim,
                           Material *mat)
    : SceneNode(name), m_primitive(prim) {
  m_nodeType = NodeType::GeometryNode;
  setMaterial(mat);
}

void GeometryNode::setMaterial(Material *mat) { m_primitive.setMaterial(mat); }
