// Termm-Fall 2023

#pragma once

#include "SceneNode.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string &meshId, const std::string &name);

  Material material;

  // Mesh Identifier. This must correspond to an object name of
  // a loaded .obj file.
  std::string meshId;
  void render(const ShaderProgram &m_shader, const BatchInfoMap &m_batchInfoMap,
              glm::mat4 v_mat, glm::mat4 m_model,
              const bool picking = false) const override;
  void reset() override;
};
