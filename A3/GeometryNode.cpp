// Termm-Fall 2023
#include "cs488-framework/BatchInfo.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include "GeometryNode.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <iostream>
using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(const std::string &meshId, const std::string &name)
    : SceneNode(name), meshId(meshId) {
  m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::render(const ShaderProgram &m_shader,
                          const BatchInfoMap &m_batchInfoMap, glm::mat4 m_view,
                          glm::mat4 m_model, const bool picking) const {
  // Update transformation matrix with node's transformations
  m_model = m_model * trans;
  // Update uniforms for openGL

  if (picking) {
    // Perform rendering with unique ID material
    float r = float(m_nodeId & 0xff) / 255.0f;
    float g = float((m_nodeId >> 8) & 0xff) / 255.0f;
    float b = float((m_nodeId >> 16) & 0xff) / 255.0f;
    updateShaderUniforms(m_shader, m_view, m_model, vec3(r, g, b), picking);
  } else if (isSelected) {
    updateShaderUniforms(m_shader, m_view, m_model, vec3(1.0f, 0.0f, 0.0f));
  } else {
    updateShaderUniforms(m_shader, m_view, m_model, material.kd);
  }
  // Extract batchInfo of verticies
  BatchInfo batchInfo = m_batchInfoMap.at(meshId);
  //-- Now render the mesh:
  m_shader.enable();
  glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
  m_shader.disable();

  for (SceneNode *node : children) {
    node->render(m_shader, m_batchInfoMap, m_view, m_model, picking);
  }
  return;
}

void GeometryNode::reset() {
  for (SceneNode *node : children) {
    node->reset();
  }
  return;
}
