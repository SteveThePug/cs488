// Termm-Fall 2023

#pragma once

#include "JointNode.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "SceneNode.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stack>

struct LightSource {
  glm::vec3 position;
  glm::vec3 rgbIntensity;
};

class A3 : public CS488Window {
public:
  A3(const std::string &luaSceneFile);
  virtual ~A3();

protected:
  virtual void init() override;
  virtual void appLogic() override;
  virtual void guiLogic() override;
  virtual void draw() override;
  virtual void cleanup() override;

  //-- Virtual callback methods
  virtual bool cursorEnterWindowEvent(int entered) override;
  virtual bool mouseMoveEvent(double xPos, double yPos) override;
  virtual bool mouseButtonInputEvent(int button, int actions,
                                     int mods) override;
  virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
  virtual bool windowResizeEvent(int width, int height) override;
  virtual bool keyInputEvent(int key, int action, int mods) override;

  //-- One time initialization methods:
  void processLuaSceneFile(const std::string &filename);
  void createShaderProgram();
  void enableVertexShaderInputSlots();
  void uploadVertexDataToVbos(const MeshConsolidator &meshConsolidator);
  void mapVboDataToVertexShaderInputLocations();
  void initViewMatrix();
  void initLightSources();

  void initPerspectiveMatrix();
  void uploadCommonSceneUniforms();
  void renderSceneGraph(const SceneNode &node);
  void renderArcCircle();

  glm::mat4 m_perpsective;
  glm::mat4 m_view;

  LightSource m_light;

  //-- GL resources for mesh geometry data:
  GLuint m_vao_meshData;
  GLuint m_vbo_vertexPositions;
  GLuint m_vbo_vertexNormals;
  GLint m_positionAttribLocation;
  GLint m_normalAttribLocation;
  ShaderProgram m_shader;

  //-- GL resources for trackball circle geometry:
  GLuint m_vbo_arcCircle;
  GLuint m_vao_arcCircle;
  GLint m_arc_positionAttribLocation;
  ShaderProgram m_shader_arcCircle;

  // BatchInfoMap is an associative container that maps a unique MeshId to a
  // BatchInfo object. Each BatchInfo object contains an index offset and the
  // number of indices required to render the mesh with identifier MeshId.
  BatchInfoMap m_batchInfoMap;

  std::string m_luaSceneFile;

  std::shared_ptr<SceneNode> m_rootNode;

  // ImGui
  bool applicationMenu, optionMenu, editMenu, jointMenu;
  void resetMenuOptions();
  void showMenuBar();

  // Interaction mode
  enum interaction_MODE { INTERACTION_POSITION, INTERACTION_JOINTS };
  int interaction_mode;

  // Picking
  bool picking;

  // Options
  bool option_circle;
  bool option_zBuffer;
  bool option_backfaceCulling;
  bool option_frontfaceCulling;
  bool option_play;
  void option_menu();

  // Edit mode
  struct edit_Prop {
    float x, y;
    JointNode *joint;
  };
  std::vector<std::vector<edit_Prop>> edit_stack;
  void edit_applyProp(edit_Prop prop);
  void edit_applyEdit(std::vector<edit_Prop> edit);
  void edit_undo(int index);
  void edit_redo(int index);
  void edit_addEdit();
  void edit_init();
  void edit_reset();
  void edit_menu();
  int edit_selected;

  // Trackball rotation
  float trackball_x, trackball_y,
      trackball_z;                             // Rotate puppet
  void trackball_update(double dz);            // Increment trackball
  void trackball_update(double dx, double dy); // Increment trackball
  void trackball_reset();                      // Reset rotation

  // Puppet
  void puppet_play();
  glm::vec3 puppet_position; // Position of the puppet
  glm::mat4 puppet_trans; // Transformation of the pupper (location + trackball)
  JointNode *puppet_head; // Head
  std::set<JointNode *> puppet_joints;
  void puppet_updateTrans();
  void puppet_resetJoints();
  void puppet_resetPosition();
  void puppet_resetOrientation();
  void puppet_resetAll();

  // Mouse input
  float mouse_x, mouse_y;
  bool mouse_left, mouse_right, mouse_middle;
  bool mouse_inCircle(int width, int height, int xPos, int yPos);

  // Picking
};
