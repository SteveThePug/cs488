// Termm-Fall 2023

#pragma once

#include "SceneNode.hpp"

class JointNode : public SceneNode {
public:
  JointNode(const std::string &name);
  virtual ~JointNode();

  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  float m_current_rotation_x, m_current_rotation_y;

  struct JointRange {
    double min, init, max;
  };

  void render(const ShaderProgram &m_shader, const BatchInfoMap &m_batchInfoMap,
              glm::mat4 v_mat, glm::mat4 m_model,
              const bool picking = false) const override;
  void reset() override;

  JointRange m_joint_x, m_joint_y;
};
