// Termm-Fall 2023

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>

using namespace glm;

static bool show_gui = true;
static float debug_time = 0.0f;
const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string &luaSceneFile)
    : m_luaSceneFile(luaSceneFile), m_positionAttribLocation(0),
      m_normalAttribLocation(0), m_vao_meshData(0), m_vbo_vertexPositions(0),
      m_vbo_vertexNormals(0), m_vao_arcCircle(0), m_vbo_arcCircle(0) {}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3() {}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init() {
  // Set the background colour.
  glClearColor(0, 0.09, 0.1, 1.0);

  createShaderProgram();

  glGenVertexArrays(1, &m_vao_arcCircle);
  glGenVertexArrays(1, &m_vao_meshData);
  enableVertexShaderInputSlots();

  processLuaSceneFile(m_luaSceneFile);

  // Load and decode all .obj files at once here.  You may add additional .obj
  // files to this list in order to support rendering additional mesh types. All
  // vertex positions, and normals will be extracted and stored within the
  // MeshConsolidator class.
  unique_ptr<MeshConsolidator> meshConsolidator(new MeshConsolidator{
      getAssetFilePath("obj/cube.obj"), getAssetFilePath("obj/sphere.obj"),
      getAssetFilePath("obj/suzanne.obj"), getAssetFilePath("obj/Body.obj"),
      getAssetFilePath("obj/Head.obj"), getAssetFilePath("obj/Eye.obj"),
      getAssetFilePath("obj/Joint.obj"), getAssetFilePath("obj/Pupil.obj"),
      getAssetFilePath("obj/Mouth.obj")});

  // Acquire the BatchInfoMap from the MeshConsolidator.
  meshConsolidator->getBatchInfoMap(m_batchInfoMap);

  // Take all vertex data within the MeshConsolidator and upload it to VBOs on
  // the GPU.
  uploadVertexDataToVbos(*meshConsolidator);

  mapVboDataToVertexShaderInputLocations();

  initPerspectiveMatrix();

  initViewMatrix();

  initLightSources();

  // Exiting the current scope calls delete automatically on meshConsolidator
  // freeing all vertex data resources.  This is fine since we already copied
  // this data to VBOs on the GPU.  We have no use for storing vertex data on
  // the CPU side beyond this point.

  // Initialisation of variables
  for (SceneNode *node : m_rootNode->get_joints()) {
    JointNode *joint = dynamic_cast<JointNode *>(node);
    puppet_joints.insert(joint);
  }
  puppet_resetAll();
  SceneNode *puppet_headNode = m_rootNode->get_child("headJoint");
  puppet_head = dynamic_cast<JointNode *>(puppet_headNode);
  option_backfaceCulling = option_zBuffer = true;
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string &filename) {
  // This version of the code treats the Lua file as an Asset,
  // so that you'd launch the program with just the filename
  // of a puppet in the Assets/ directory.
  // std::string assetFilePath = getAssetFilePath(filename.c_str());
  // m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

  // This version of the code treats the main program argument
  // as a straightforward pathname.
  m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
  if (!m_rootNode) {
    std::cerr << "Could Not Open " << filename << std::endl;
  }
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram() {
  m_shader.generateProgramObject();
  m_shader.attachVertexShader(getAssetFilePath("shaders/Phong.vs").c_str());
  m_shader.attachFragmentShader(getAssetFilePath("shaders/Phong.fs").c_str());
  m_shader.link();

  m_shader_arcCircle.generateProgramObject();
  m_shader_arcCircle.attachVertexShader(
      getAssetFilePath("shaders/arc_VertexShader.vs").c_str());
  m_shader_arcCircle.attachFragmentShader(
      getAssetFilePath("shaders/arc_FragmentShader.fs").c_str());
  m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots() {
  //-- Enable input slots for m_vao_meshData:
  {
    glBindVertexArray(m_vao_meshData);

    // Enable the vertex shader attribute location for "position" when
    // rendering.
    m_positionAttribLocation = m_shader.getAttribLocation("position");
    glEnableVertexAttribArray(m_positionAttribLocation);

    // Enable the vertex shader attribute location for "normal" when rendering.
    m_normalAttribLocation = m_shader.getAttribLocation("normal");
    glEnableVertexAttribArray(m_normalAttribLocation);

    CHECK_GL_ERRORS;
  }

  //-- Enable input slots for m_vao_arcCircle:
  {
    glBindVertexArray(m_vao_arcCircle);

    // Enable the vertex shader attribute location for "position" when
    // rendering.
    m_arc_positionAttribLocation =
        m_shader_arcCircle.getAttribLocation("position");
    glEnableVertexAttribArray(m_arc_positionAttribLocation);

    CHECK_GL_ERRORS;
  }

  // Restore defaults
  glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos(const MeshConsolidator &meshConsolidator) {
  // Generate VBO to store all vertex position data
  {
    glGenBuffers(1, &m_vbo_vertexPositions);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

    glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
                 meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERRORS;
  }

  // Generate VBO to store all vertex normal data
  {
    glGenBuffers(1, &m_vbo_vertexNormals);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

    glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
                 meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERRORS;
  }

  // Generate VBO to store the trackball circle.
  {
    glGenBuffers(1, &m_vbo_arcCircle);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);

    float *pts = new float[2 * CIRCLE_PTS];
    for (size_t idx = 0; idx < CIRCLE_PTS; ++idx) {
      float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
      pts[2 * idx] = cos(ang);
      pts[2 * idx + 1] = sin(ang);
    }

    glBufferData(GL_ARRAY_BUFFER, 2 * CIRCLE_PTS * sizeof(float), pts,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERRORS;
  }
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations() {
  // Bind VAO in order to record the data mapping.
  glBindVertexArray(m_vao_meshData);

  // Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into
  // the "position" vertex attribute location for any bound vertex shader
  // program.
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
  glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0,
                        nullptr);

  // Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into
  // the "normal" vertex attribute location for any bound vertex shader program.
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
  glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0,
                        nullptr);

  //-- Unbind target, and restore default values:
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  CHECK_GL_ERRORS;

  // Bind VAO in order to record the data mapping.
  glBindVertexArray(m_vao_arcCircle);

  // Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
  // "position" vertex attribute location for any bound vertex shader program.
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
  glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0,
                        nullptr);

  //-- Unbind target, and restore default values:
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix() {
  float aspect = ((float)m_windowWidth) / m_windowHeight;
  m_perpsective =
      glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}

//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
  m_view = glm::lookAt(vec3(0.0f, 0.0f, 4.0f), vec3(0.0f, 0.0f, 0.0f),
                       vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
  // World-space position
  m_light.position = vec3(2.0f, 2.0f, 2.0f);
  m_light.rgbIntensity = vec3(1.0f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
  m_shader.enable();
  {
    //-- Set Perpsective matrix uniform for the scene:
    GLint location = m_shader.getUniformLocation("Perspective");
    glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
    CHECK_GL_ERRORS;

    location = m_shader.getUniformLocation("picking");
    glUniform1i(location, picking ? 1 : 0);

    if (!picking) {
      //-- Set LightSource uniform for the scene:
      location = m_shader.getUniformLocation("light.position");
      glUniform3fv(location, 1, value_ptr(m_light.position));
      location = m_shader.getUniformLocation("light.rgbIntensity");
      glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
      CHECK_GL_ERRORS;

      location = m_shader.getUniformLocation("ambientIntensity");
      vec3 ambientIntensity(0.25f);
      glUniform3fv(location, 1, value_ptr(ambientIntensity));
      CHECK_GL_ERRORS;
    }
  }
  m_shader.disable();
}

void A3::puppet_play() {
  for (JointNode *joint : puppet_joints) {

    string name = joint->m_name;
    if (name != "l_pupilJoint" && name != "r_pupilJoint" && name != "headJoint")
      continue;

    auto lambdaFunc = [](float &currentValue, float minVal, float maxVal,
                         float &step) {
      // Update the value
      currentValue += step;

      // Check and reverse the direction at the boundaries
      if (currentValue <= minVal || currentValue >= maxVal) {
        step = -step; // Reverse direction
      }

      if (currentValue < minVal)
        currentValue = minVal;
      if (currentValue > maxVal)
        currentValue = maxVal;
    };

    float min_x, max_x, min_y, max_y;

    min_x = joint->m_joint_x.min;
    max_x = joint->m_joint_x.max;
    static float step_x = 0.2;
    lambdaFunc(joint->m_current_rotation_x, min_x, max_x, step_x);

    min_y = joint->m_joint_y.min;
    max_y = joint->m_joint_y.max;
    static float step_y = 0.81234214;
    lambdaFunc(joint->m_current_rotation_y, min_y, max_y, step_y);
  }
}

void A3::appLogic() {
  // Update puppet transformation
  puppet_updateTrans();
  if (option_play)
    puppet_play();
  uploadCommonSceneUniforms();
  debug_time += 0.01;
}

void A3::resetMenuOptions() {
  applicationMenu = false;
  optionMenu = false;
  editMenu = false;
  jointMenu = false;
}

void A3::showMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::Button("App")) {
      resetMenuOptions();
      applicationMenu = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Edit Menu")) {
      resetMenuOptions();
      editMenu = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Option Menu")) {
      resetMenuOptions();
      optionMenu = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Joint Menu")) {
      resetMenuOptions();
      jointMenu = true;
    }
  }
  ImGui::EndMainMenuBar();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic() {
  if (!show_gui) {
    return;
  }

  static bool firstRun(true);
  if (firstRun) {
    ImGui::SetNextWindowPos(ImVec2(50, 50));
    firstRun = false;
    applicationMenu = true;
    optionMenu = false;
    editMenu = false;
  }

  static bool showDebugWindow(true);
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize;

  float opacity(0.5f);

  ImGui::Begin("", &showDebugWindow, ImVec2(100, 100), opacity, windowFlags);
  showMenuBar();

  ImGui::Spacing();

  ImGui::RadioButton("Position", &interaction_mode, INTERACTION_POSITION);
  ImGui::RadioButton("Joints", &interaction_mode, INTERACTION_JOINTS);

  if (applicationMenu) {
    if (ImGui::Button("Reset Position")) {
      puppet_resetPosition();
    }
    if (ImGui::Button("Reset Orientation")) {
      puppet_resetOrientation();
    }
    if (ImGui::Button("Reset Joints")) {
      puppet_resetJoints();
    }
    if (ImGui::Button("Reset All")) {
      puppet_resetAll();
    }
    if (ImGui::Button("Quit Application")) {
      glfwSetWindowShouldClose(m_window, GL_TRUE);
    }
  }
  if (jointMenu) {
    ImGui::Text("Current Joint Values");
    for (JointNode *joint : puppet_joints) {
      float min_x = joint->m_joint_x.min;
      float min_y = joint->m_joint_y.min;
      float max_x = joint->m_joint_x.max;
      float max_y = joint->m_joint_y.max;
      if (min_x != max_x)
        if (ImGui::SliderFloat((joint->m_name + " X").c_str(),
                               &joint->m_current_rotation_x, min_x, max_x)) {
        }
      if (min_y != max_y)
        if (ImGui::SliderFloat((joint->m_name + " Y").c_str(),
                               &joint->m_current_rotation_y, min_y, max_y)) {
        }
    }
  }
  if (optionMenu) {
    option_menu();
  }
  if (editMenu) {
    edit_menu();
  }

  ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);
  ImGui::End();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

  if (option_zBuffer) {
    glEnable(GL_DEPTH_TEST);
  }
  if (option_frontfaceCulling && option_backfaceCulling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT_AND_BACK);
  } else if (option_frontfaceCulling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
  } else if (option_backfaceCulling) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  } else {
    glDisable(GL_CULL_FACE);
  }

  renderSceneGraph(*m_rootNode);

  glDisable(GL_DEPTH_TEST);

  if (option_circle ||
      (mouse_right && interaction_mode == INTERACTION_POSITION))
    renderArcCircle();
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode &root) {

  // Bind the VAO once here, and reuse for all GeometryNode rendering below.
  glBindVertexArray(m_vao_meshData);

  // This is emphatically *not* how you should be drawing the scene graph in
  // your final implementation.  This is a non-hierarchical demonstration
  // in which we assume that there is a list of GeometryNodes living directly
  // underneath the root node, and that we can draw them in a loop.  It's
  // just enough to demonstrate how to get geometry and materials out of
  // a GeometryNode and onto the screen.

  // You'll want to turn this into recursive code that walks over the tree.
  // You can do that by putting a method in SceneNode, overridden in its
  // subclasses, that renders the subtree rooted at every node.  Or you
  // could put a set of mutually recursive functions in this class, which
  // walk down the tree from nodes of different types.

  root.render(m_shader, m_batchInfoMap, m_view, puppet_trans, picking);

  glBindVertexArray(0);
  CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
  glBindVertexArray(m_vao_arcCircle);

  m_shader_arcCircle.enable();
  GLint m_location = m_shader_arcCircle.getUniformLocation("M");
  float aspect = float(m_framebufferWidth) / float(m_framebufferHeight);
  glm::mat4 M;
  if (aspect > 1.0) {
    M = glm::scale(glm::mat4(), glm::vec3(0.5 / aspect, 0.5, 1.0));
  } else {
    M = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5 * aspect, 1.0));
  }
  glUniformMatrix4fv(m_location, 1, GL_FALSE, value_ptr(M));
  glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_PTS);
  m_shader_arcCircle.disable();

  glBindVertexArray(0);
  CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup() {}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent(int entered) {
  bool eventHandled(false);

  // Fill in with event handling code...

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent(double xPos, double yPos) {
  bool eventHandled(false);
  // dx and dy between 0 and 1
  float dx = -(mouse_x - xPos) / m_windowWidth;
  float dy = (mouse_y - yPos) / m_windowHeight;
  mouse_x = xPos;
  mouse_y = yPos;

  if (interaction_mode == INTERACTION_POSITION) {
    if (mouse_left) {
      puppet_position.x += dx * 3;
      puppet_position.y += dy * 3;
    }
    if (mouse_middle) {
      puppet_position.z += dx * 3;
    }
    if (mouse_right) {
      if (mouse_inCircle(m_windowWidth, m_windowHeight, xPos, yPos)) {
        trackball_update(-dy * 100, dx * 100);
      } else {
        trackball_update(-dx);
      }
    }
    eventHandled = true;
  }

  if (interaction_mode == INTERACTION_JOINTS) {
    if (mouse_left || mouse_right || mouse_middle) {
      if (mouse_right) {
        if (puppet_head) {
          puppet_head->m_current_rotation_y += dx * 10;
        }
      }
      for (JointNode *joint : puppet_joints) {
        bool activeJoint = false;

        // Check if we have an active child
        for (SceneNode *child : joint->children) {
          if (child->isSelected == true) {
            activeJoint = true;
          }
        }

        if (!activeJoint)
          continue;

        if (mouse_middle && joint) {
          joint->m_current_rotation_x -= dy * 100;
          joint->m_current_rotation_y += dx * 100;
        }
      }
      eventHandled = true;
    }
  }

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent(int button, int actions, int mods) {
  bool eventHandled(false);

  if (!ImGui::IsMouseHoveringAnyWindow()) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      if (actions == GLFW_PRESS) {
        mouse_left = true;
        if (interaction_mode == INTERACTION_JOINTS) {
          // Render the SceneNode for picking
          picking = true;
          uploadCommonSceneUniforms();
          glClearColor(1.0, 1.0, 1.0, 1.0);
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          glClearColor(0, 0.09, 0.1, 1.0);

          glEnable(GL_DEPTH_TEST);
          renderSceneGraph(*m_rootNode);
          glDisable(GL_DEPTH_TEST);
          CHECK_GL_ERRORS;

          // We need to read it using these coords (apparently)
          // X position
          double xpos, ypos;
          glfwGetCursorPos(m_window, &xpos, &ypos);
          // Ugly -- FB coordinates might be different than Window coordinates
          // (e.g., on a retina display).  Must compensate.
          xpos *= double(m_framebufferWidth) / double(m_windowWidth);
          // WTF, don't know why I have to measure y relative to the bottom of
          // the window in this case.
          ypos = m_windowHeight - ypos;
          ypos *= double(m_framebufferHeight) / double(m_windowHeight);

          GLubyte buffer[4] = {0, 0, 0, 0};
          glReadBuffer(GL_BACK);
          glReadPixels(int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                       buffer);
          unsigned int id = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);

          SceneNode *node = m_rootNode->get_child(id);
          if (node) {
            node->isSelected = !node->isSelected;
          }

          picking = false;
          CHECK_GL_ERRORS;
        }

        eventHandled = true;
      } else if (actions == GLFW_RELEASE) {
        mouse_left = false;
        eventHandled = true;
      }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (actions == GLFW_PRESS) {
        mouse_right = true;
        eventHandled = true;
      } else if (actions == GLFW_RELEASE) {
        mouse_right = false;
        eventHandled = true;
      }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (actions == GLFW_PRESS) {
        mouse_middle = true;
        eventHandled = true;
      } else if (actions == GLFW_RELEASE) {
        if (interaction_mode == INTERACTION_JOINTS) {
          edit_addEdit();
        }
        mouse_middle = false;
        eventHandled = true;
      }
    }
  }
  // Fill in with event handling code...

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent(double xOffSet, double yOffSet) {
  bool eventHandled(false);

  // Fill in with event handling code...

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent(int width, int height) {
  bool eventHandled(false);
  initPerspectiveMatrix();
  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent(int key, int action, int mods) {
  bool eventHandled(false);

  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_M) {
      show_gui = !show_gui;
      eventHandled = true;
    }
    if (key == GLFW_KEY_Q) {
      glfwSetWindowShouldClose(m_window, GL_TRUE);
      eventHandled = true;
    }
    if (key == GLFW_KEY_A) {
      puppet_resetAll();
      eventHandled = true;
    }
    if (key == GLFW_KEY_I) {
      puppet_resetPosition();
      eventHandled = true;
    }
    if (key == GLFW_KEY_S) {
      puppet_resetJoints();
      eventHandled = true;
    }
    if (key == GLFW_KEY_C) {
      option_circle = !option_circle;
      eventHandled = true;
    }
    if (key == GLFW_KEY_Z) {
      option_zBuffer = !option_zBuffer;
      eventHandled = true;
    }
    if (key == GLFW_KEY_B) {
      option_backfaceCulling = !option_backfaceCulling;
      eventHandled = true;
    }
    if (key == GLFW_KEY_F) {
      option_frontfaceCulling = !option_frontfaceCulling;
      eventHandled = true;
    }
    if (key == GLFW_KEY_P) {
      interaction_mode = INTERACTION_POSITION;
      eventHandled = true;
    }
    if (key == GLFW_KEY_J) {
      interaction_mode = INTERACTION_JOINTS;
      eventHandled = true;
    }
  }
  // Fill in with event handling code...

  return eventHandled;
}

void A3::puppet_resetPosition() {
  puppet_position = vec3(0.0f, 0.0f, 0.0f);
  return;
}
void A3::puppet_resetOrientation() {
  trackball_y = 0.0f;
  trackball_x = 0.0f;
  trackball_z = 0.0f;
  return;
}
void A3::puppet_resetJoints() {
  m_rootNode->reset();
  edit_reset();
}
void A3::puppet_resetAll() {
  puppet_resetPosition();
  puppet_resetOrientation();
  puppet_resetJoints();
  puppet_updateTrans();
}
void A3::puppet_updateTrans() {
  mat4 trans; // Identity matrix
  // Translate for pupper position
  trans = translate(trans, puppet_position);

  vec4 norm = vec4(0.0f, 0.0f, 1.0f, 1.0f);

  // Rotate around the x-axis
  trans = glm::rotate(trans, trackball_x, glm::vec3(1.0f, 0.0f, 0.0f));

  // Rotate around the y-axis
  trans = glm::rotate(trans, trackball_y, glm::vec3(0.0f, 1.0f, 0.0f));

  // Rotate around the z-axis
  norm = norm * trans;
  trans = glm::rotate(trans, trackball_z, vec3(norm));

  puppet_trans = trans;
}

// Trackball code
void A3::trackball_update(double dz) {
  trackball_z += dz;
  return;
}
void A3::trackball_update(double dx, double dy) {
  trackball_x += dx / 100;
  trackball_y += dy / 100;
  return;
}

void A3::trackball_reset() { trackball_z = trackball_x = trackball_z = 0; }

bool A3::mouse_inCircle(int width, int height, int xPos, int yPos) {
  int radius;
  if (width < height) {
    radius = width / 4;
  } else {
    radius = height / 4;
  }
  vec2 center = vec2(width / 2, height / 2);
  vec2 mousePos = vec2(xPos, yPos);
  if (length(center - mousePos) < radius)
    return true;
  return false;
}

void A3::edit_reset() {
  edit_stack.clear();
  edit_init();
}
void A3::edit_init() {
  edit_selected = 0;
  vector<edit_Prop> edit;
  for (JointNode *joint : puppet_joints) {
    edit_Prop editProp;
    editProp.x = joint->m_joint_x.init;
    editProp.y = joint->m_joint_y.init;
    editProp.joint = joint;
    edit.push_back(editProp);
  }
  edit_stack.push_back(edit);
}
void A3::edit_applyProp(edit_Prop prop) {
  prop.joint->m_current_rotation_x = prop.x;
  prop.joint->m_current_rotation_y = prop.y;
}
void A3::edit_applyEdit(vector<edit_Prop> edit) {
  for (edit_Prop prop : edit) {
    edit_applyProp(prop);
  }
}
void A3::edit_redo(int index) {
  edit_selected = index;
  for (int i = 0; i <= index; i++) {
    edit_applyEdit(edit_stack[i]);
  }
}
void A3::edit_undo(int index) {
  edit_selected = index - 1;
  for (int i = 0; i < index; i++) {
    edit_applyEdit(edit_stack[i]);
  }
}
void A3::edit_addEdit() {
  vector<edit_Prop> edit;

  for (JointNode *joint : puppet_joints) {
    bool activeJoint = false;

    // Check if we have an active child
    for (SceneNode *child : joint->children) {
      if (child->isSelected == true) {
        activeJoint = true;
      }
    }
    if (!activeJoint)
      continue;

    edit_Prop editProp;
    editProp.x = joint->m_current_rotation_x;
    editProp.y = joint->m_current_rotation_y;
    editProp.joint = joint;
    edit.push_back(editProp);
  }

  if (edit.size() == 0)
    return;

  int toPop = edit_stack.size() - edit_selected - 1;
  for (int i = 0; i < toPop; i++) {
    edit_stack.pop_back();
  }

  edit_selected = edit_stack.size();
  edit_stack.push_back(edit);
}
void A3::edit_menu() {
  int edit_index = 0;
  for (vector<edit_Prop> edit : edit_stack) {
    if (edit_index == edit_selected) {
      ImGui::Bullet();
      ImGui::SameLine();
    }
    stringstream ss;
    ss << "Undo " << edit_index;
    if (ImGui::Button(ss.str().c_str())) {
      edit_undo(edit_index);
    }
    ImGui::SameLine();
    ss.str("");
    ss << "Redo " << edit_index;
    if (ImGui::Button(ss.str().c_str())) {
      edit_redo(edit_index);
    }
    ImGui::SameLine();
    ss.str("");
    ss << "Edit " << edit_index;
    if (ImGui::BeginMenu(ss.str().c_str())) {
      for (edit_Prop prop : edit) {
        ss.str("");
        ss << "X: " << prop.x << "  ";
        ss << "X: " << prop.x << "  ";
        ss << " Y: " << prop.y << "  ";
        ss << "Joint: " << prop.joint->m_name;
        ImGui::Text("%s", ss.str().c_str());
      }
      ImGui::EndMenu();
    }
    edit_index++;
  }
}
void A3::option_menu() {
  ImGui::Checkbox("Circle", &option_circle);
  ImGui::Checkbox("Z-buffer", &option_zBuffer);
  ImGui::Checkbox("Backface-Culling", &option_backfaceCulling);
  ImGui::Checkbox("Frontface-Culling", &option_frontfaceCulling);
  ImGui::Checkbox("Play", &option_play);
}
