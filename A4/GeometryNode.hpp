// Termm--Fall 2023

#pragma once

#include "Material.hpp"
#include "Primitive.hpp"
#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
  Primitive &m_primitive;

  GeometryNode(const std::string &name, Primitive &prim);
  GeometryNode(const std::string &name, Primitive &prim, Material *mat);

  void setMaterial(Material *material);

  void print(std::ostream &os) const override;
};
