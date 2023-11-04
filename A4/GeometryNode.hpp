// Termm--Fall 2023

#pragma once

#include "Material.hpp"
#include "Primitive.hpp"
#include "SceneNode.hpp"

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string &name, Primitive *prim);
  GeometryNode(const std::string &name, Primitive *prim, Material *mat);

  void setMaterial(Material *material);

  Material *m_material;
  Primitive *m_primitive;
};
