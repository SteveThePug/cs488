// Termm--Fall 2023

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

// Line Struct
//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData() : numVertices(0), index(0) {
  positions.resize(kMaxVertices);
  colours.resize(kMaxVertices);
}

//----------------------------------------------------------------------------------------
// Constructor
A2::A2() : m_currentLineColour(vec3(0.0f)) {}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2() {}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init() {
  // My initialisations
  initControl();

  // Set the background colour.
  glClearColor(0.2, 0.5, 0.3, 1.0);

  createShaderProgram();

  glGenVertexArrays(1, &m_vao);

  enableVertexAttribIndices();

  generateVertexBuffers();

  mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram() {
  m_shader.generateProgramObject();
  m_shader.attachVertexShader(getAssetFilePath("VertexShader.vs").c_str());
  m_shader.attachFragmentShader(getAssetFilePath("FragmentShader.fs").c_str());
  m_shader.link();
}

//----------------------------------------------------------------------------------------
// Fall 2022
void A2::enableVertexAttribIndices() {
  glBindVertexArray(m_vao);

  // Enable the attribute index location for "position" when rendering.
  GLint positionAttribLocation = m_shader.getAttribLocation("position");
  glEnableVertexAttribArray(positionAttribLocation);

  // Enable the attribute index location for "colour" when rendering.
  GLint colourAttribLocation = m_shader.getAttribLocation("colour");
  glEnableVertexAttribArray(colourAttribLocation);

  // Restore defaults
  glBindVertexArray(0);

  CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers() {
  // Generate a vertex buffer to store line vertex positions
  {
    glGenBuffers(1, &m_vbo_positions);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

    // Set to GL_DYNAMIC_DRAW because the data store will be modified
    // frequently.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
                 GL_DYNAMIC_DRAW);

    // Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
  }

  // Generate a vertex buffer to store line colors
  {
    glGenBuffers(1, &m_vbo_colours);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

    // Set to GL_DYNAMIC_DRAW because the data store will be modified
    // frequently.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
                 GL_DYNAMIC_DRAW);

    // Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
  }
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation() {
  // Bind VAO in order to record the data mapping.
  glBindVertexArray(m_vao);

  // Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
  // "position" vertex attribute index for any bound shader program.
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
  GLint positionAttribLocation = m_shader.getAttribLocation("position");
  glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0,
                        nullptr);

  // Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
  // "colour" vertex attribute index for any bound shader program.
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
  GLint colorAttribLocation = m_shader.getAttribLocation("colour");
  glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  //-- Unbind target, and restore default values:
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData() {
  m_vertexData.numVertices = 0;
  m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour(const vec3 &colour) { m_currentLineColour = colour; }

//---------------------------------------------------------------------------------------
void A2::drawLine(const vec2 &V0, // Line Start (NDC coordinate)
                  const vec2 &V1  // Line End (NDC coordinate)
) {

  m_vertexData.positions[m_vertexData.index] = V0;
  m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
  ++m_vertexData.index;
  m_vertexData.positions[m_vertexData.index] = V1;
  m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
  ++m_vertexData.index;

  m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic() {
  // Place per frame, application logic here ...

  // Call at the beginning of frame, before drawing lines:
  initLineData();

  // All Lines
  vector<Line3D> lines;

  // Make Cube
  vector<Line3D> cube = generateCubeArray();
  vector<Line3D> cubeGnomon =
      generateGnomon(COLOR_RED, COLOR_BLUE, COLOR_GREEN);

  // Transform Cube w/ Mod`el Matrix
  mat4 cubeModelMatrix = genRotationMatrix(modelYaw, modelPitch, modelRoll);
  cubeModelMatrix = genScaleMatrix(modelScale) * cubeModelMatrix;
  cubeModelMatrix = genTranslationMatrix(modelPos) * cubeModelMatrix;
  cube = transform3DLines(cube, cubeModelMatrix);
  cubeGnomon = transform3DLines(cubeGnomon, cubeModelMatrix);

  // Add cube to allLines
  lines.insert(lines.end(), cube.begin(), cube.end());
  lines.insert(lines.end(), cubeGnomon.begin(), cubeGnomon.end());

  // World Gnomon
  vector<Line3D> worldGnomon =
      generateGnomon(COLOR_WHITE, COLOR_RED, COLOR_GREEN);
  lines.insert(lines.end(), worldGnomon.begin(), worldGnomon.end());

  // View transformation
  mat4 viewMatrix = genViewMatrix(eyePos, objPos, eyeUp);
  viewMatrix = genRotationMatrix(eyeYaw, eyePitch, eyeRoll) * viewMatrix;
  lines = transform3DLines(lines, viewMatrix);

  // Clip Near and Far so we dont divide 0
  lines = clipNear(lines, eyeNear);
  lines = clipFar(lines, eyeFar);

  // Time to generate a perspective projection
  float width = viewport.topLeft.x - viewport.bottomRight.x;
  float height = viewport.bottomRight.y - viewport.topLeft.y;
  mat4 projectionMatrix =
      genProjectionMatrix(eyeFov, width / height, eyeNear, eyeFar);
  lines = transform3DLines(lines, projectionMatrix);
  lines = homogenizeLines(lines);

  // Clip to unit cube
  lines = clipLeft(lines, -1);
  lines = clipRight(lines, 1);
  lines = clipBottom(lines, -1);
  lines = clipTop(lines, 1);

  // Transform to viewport
  mat4 viewportMatrix = genViewportMatrix(viewport);
  lines = transform3DLines(lines, viewportMatrix);

  // Draw lines:
  draw3DLineArray(lines);
  drawViewport(viewport);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic() {
  static bool firstRun(true);
  if (firstRun) {
    ImGui::SetNextWindowPos(ImVec2(50, 50));
    firstRun = false;
  }

  static bool showDebugWindow(true);
  ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
  float opacity(0.5f);

  ImGui::Begin("Properties", &showDebugWindow, ImVec2(100, 100), opacity,
               windowFlags);

  ImGui::Text("Near = %f", eyeNear);
  ImGui::Text("Far =  %f", eyeFar);

  ImGui::RadioButton("Rotate View (o)", &interactionMode, ROTATE_VIEW);
  ImGui::RadioButton("Translate View (e)", &interactionMode, TRANSLATE_VIEW);
  ImGui::RadioButton("Perspective View (p)", &interactionMode,
                     PERSPECTIVE_VIEW);
  ImGui::RadioButton("Rotate Model (r)", &interactionMode, ROTATE_MODEL);
  ImGui::RadioButton("Translate Model (t)", &interactionMode, TRANSLATE_MODEL);
  ImGui::RadioButton("Scale Model (s)", &interactionMode, SCALE_MODEL);
  ImGui::RadioButton("Viewport (v)", &interactionMode, VIEWPORT);

  // Create Button, and check if it was clicked:
  if (ImGui::Button("Quit Application (q)")) {
    glfwSetWindowShouldClose(m_window, GL_TRUE);
  }
  if (ImGui::Button("Reset (a)")) {
    initControl();
  }

  ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

  ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

  //-- Copy vertex position data into VBO, m_vbo_positions:
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
                    m_vertexData.positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
  }

  //-- Copy vertex colour data into VBO, m_vbo_colours:
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
                    m_vertexData.colours.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
  }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw() {
  uploadVertexDataToVbos();

  glBindVertexArray(m_vao);

  m_shader.enable();
  glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
  m_shader.disable();

  // Restore defaults
  glBindVertexArray(0);

  CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup() {}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent(int entered) {
  bool eventHandled(false);

  // Fill in with event handling code...

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent(double xPos, double yPos) {
  bool eventHandled(false);
  float dx = (mouse_x - xPos) / 100;
  dx += (mouse_y - yPos) / 100;

  if (interactionMode == ROTATE_VIEW) {
    if (mouse_left) {
      eyeYaw += dx;
    } else if (mouse_middle) {
      eyePitch += dx;
    } else if (mouse_right) {
      eyeRoll += dx;
    }
    eventHandled = true;
    // Middle Vertical Axis
    // Left Horizontal Axis
    // Right Z axis
    eventHandled = true;
  } else if (interactionMode == TRANSLATE_VIEW) {
    if (mouse_left) {
      vec3 right = cross(eyeUp, objPos - eyePos);
      eyePos += right * dx;
      objPos += right * dx;
    } else if (mouse_middle) {
      vec3 right = cross(eyeUp, objPos - eyePos);
      vec3 up = cross(objPos - eyePos, right);
      eyePos += up * dx;
      objPos += up * dx;
    } else if (mouse_right) {
      eyePos += (objPos - eyePos) * dx;
      objPos += (objPos - eyePos) * dx;
    }
    eventHandled = true;
  } else if (interactionMode == PERSPECTIVE_VIEW) {
    if (mouse_left && eyeFov + dx > 0.087f && eyeFov + dx < 2.79f)
      eyeFov += dx;
    if (mouse_middle && eyeNear + dx > 0 && eyeNear + dx < eyeFar)
      eyeNear += dx;
    if (mouse_right && eyeFar + dx > eyeNear)
      eyeFar += dx;
    eventHandled = true;
  } else if (interactionMode == ROTATE_MODEL) {
    if (mouse_left)
      modelRoll += dx;
    if (mouse_middle)
      modelPitch += dx;
    if (mouse_right)
      modelYaw += dx;
  } else if (interactionMode == TRANSLATE_MODEL) {
    if (mouse_left)
      modelPos.x += dx;
    if (mouse_middle)
      modelPos.y += dx;
    if (mouse_right)
      modelPos.z += dx;
    eventHandled = true;
  } else if (interactionMode == SCALE_MODEL) {
    if (mouse_left)
      modelScale.x += dx;
    if (mouse_middle)
      modelScale.y += dx;
    if (mouse_right)
      modelScale.z += dx;
    eventHandled = true;
  } else if (interactionMode == VIEWPORT) {
    if (mouse_left) {
      float a = 2.0f * (mouse_x / m_windowWidth) - 1.0f;
      float b = -2.0f * (mouse_y / m_windowHeight) + 1.0f;
      viewport.bottomRight = vec2(a, b);
    }
    eventHandled = true;
  }

  mouse_x = xPos;
  mouse_y = yPos;
  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent(int button, int actions, int mods) {
  bool eventHandled(false);

  if (!ImGui::IsMouseHoveringAnyWindow()) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      if (actions == GLFW_PRESS and interactionMode == VIEWPORT) {
        float a = 2.0f * (mouse_x / m_windowWidth) - 1.0f;
        float b = -2.0f * (mouse_y / m_windowHeight) + 1.0f;
        viewport.topLeft = vec2(a, b);
        viewport.bottomRight = vec2(a + 0.01, b - 0.01f);
        mouse_left = true;
        eventHandled = true;
      } else if (actions == GLFW_PRESS) {
        mouse_left = true;
        eventHandled = true;
      } else if (actions == GLFW_RELEASE) {
        mouse_left = false;
        eventHandled = true;
      }
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
      if (actions == GLFW_PRESS) {
        mouse_middle = true;
        eventHandled = true;
      } else if (actions == GLFW_RELEASE) {
        mouse_middle = false;
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
  }

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent(double xOffSet, double yOffSet) {
  bool eventHandled(false);

  // Fill in with event handling code...

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent(int width, int height) {
  bool eventHandled(false);

  // Fill in with event handling code...

  return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent(int key, int action, int mods) {
  bool eventHandled = false;

  // Fill in with event handling code...
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_Q) {
      glfwSetWindowShouldClose(m_window, GL_TRUE);
      eventHandled = true;
    }
    if (key == GLFW_KEY_A) {
      initControl();
      eventHandled = true;
    }
    if (key == GLFW_KEY_0) {
      interactionMode = ROTATE_VIEW;
      eventHandled = true;
    }
    if (key == GLFW_KEY_E) {
      interactionMode = TRANSLATE_VIEW;
      eventHandled = true;
    }
    if (key == GLFW_KEY_P) {
      interactionMode = PERSPECTIVE_VIEW;
      eventHandled = true;
    }
    if (key == GLFW_KEY_R) {
      interactionMode = ROTATE_MODEL;
      eventHandled = true;
    }
    if (key == GLFW_KEY_T) {
      interactionMode = TRANSLATE_MODEL;
      eventHandled = true;
    }
    if (key == GLFW_KEY_S) {
      interactionMode = SCALE_MODEL;
      eventHandled = true;
    }
    if (key == GLFW_KEY_V) {
      interactionMode = VIEWPORT;
      eventHandled = true;
    }
  }
  return eventHandled;
}

Line3D A2::genLine3D(vec4 start, vec4 end, vec3 color) {
  Line3D line = Line3D();
  line.start = start;
  line.end = end;
  line.color = color;
  return line;
}

vector<Line3D> A2::generateCubeArray() {
  // White color
  vec3 color = vec3(1, 1, 1);
  // Verticies
  vec4 A = vec4(-1.0f, -1.0f, -1.0f, 1.0f);
  vec4 B = vec4(-1.0f, -1.0f, 1.0f, 1.0f);
  vec4 C = vec4(-1.0f, 1.0f, 1.0f, 1.0f);
  vec4 D = vec4(-1.0f, 1.0f, -1.0f, 1.0f);
  vec4 E = vec4(1.0f, -1.0f, -1.0f, 1.0f);
  vec4 F = vec4(1.0f, -1.0f, 1.0f, 1.0f);
  vec4 H = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  vec4 G = vec4(1.0f, 1.0f, -1.0f, 1.0f);
  // Lines
  Line3D l0 = genLine3D(A, B, color);
  Line3D l1 = genLine3D(B, C, color);
  Line3D l2 = genLine3D(C, D, color);
  Line3D l3 = genLine3D(D, A, color);
  Line3D l4 = genLine3D(E, F, color);
  Line3D l5 = genLine3D(F, H, color);
  Line3D l6 = genLine3D(H, G, color);
  Line3D l7 = genLine3D(G, E, color);
  Line3D l8 = genLine3D(A, E, color);
  Line3D l9 = genLine3D(B, F, color);
  Line3D l10 = genLine3D(C, H, color);
  Line3D l11 = genLine3D(D, G, color);
  // Create Array
  vector<Line3D> lines;
  lines.push_back(l0);
  lines.push_back(l1);
  lines.push_back(l2);
  lines.push_back(l3);
  lines.push_back(l4);
  lines.push_back(l5);
  lines.push_back(l6);
  lines.push_back(l7);
  lines.push_back(l8);
  lines.push_back(l9);
  lines.push_back(l10);
  lines.push_back(l11);
  return lines;
}

vector<Line3D> A2::generateGnomon(vec3 colorX, vec3 colorY, vec3 colorZ) {
  // Verticies
  vec4 O = vec4(.0f, .0f, .0f, 1.0f);
  vec4 X = vec4(1.0f, .0f, .0f, 1.0f);
  vec4 Y = vec4(.0f, 1.0f, .0f, 1.0f);
  vec4 Z = vec4(.0f, .0f, 1.0f, 1.0f);
  // Create Lines
  Line3D l0 = genLine3D(O, X, colorX);
  Line3D l1 = genLine3D(O, Y, colorY);
  Line3D l2 = genLine3D(O, Z, colorZ);
  // Create Array
  vector<Line3D> lines;
  lines.push_back(l0);
  lines.push_back(l1);
  lines.push_back(l2);
  return lines;
}

mat4 A2::genIdentityMatrix() {
  mat4 matrix = mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                     1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  return matrix;
}

mat4 A2::genTranslationMatrix(vec3 translation) {
  mat4 matrix =
      mat4(1.0f, 0.0f, 0.0f, translation.x, 0.0f, 1.0f, 0.0f, translation.y,
           0.0f, 0.0f, 1.0f, translation.z, 0.0f, 0.0f, 0.0f, 1.0f);
  return transpose(matrix);
}

// Yaw about Z, Pitch about Y, Roll about x
mat4 A2::genRotationMatrix(float yaw, float pitch, float roll) {
  float cy = cos(yaw);
  float sy = sin(yaw);
  float cp = cos(pitch);
  float sp = sin(pitch);
  float cr = cos(roll);
  float sr = sin(roll);

  mat4 matrix =
      mat4(cy * cp, cy * sp * sr - sy * cr, cy * sp * cr + sy * sr, 0.0f,
           sy * cp, sy * sp * sr + cy * cr, sy * sp * cr - cy * sr, 0.0f, -sp,
           cp * sr, cp * cr, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  return transpose(matrix);
}

mat4 A2::genScaleMatrix(vec3 scale) {
  mat4 matrix = mat4(scale.x, 0.0f, 0.0f, 0.0f, 0.0f, scale.y, 0.0f, 0.0f, 0.0f,
                     0.0f, scale.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  return matrix;
}

mat4 A2::genPositionCamera(vec3 cameraPos) {
  // First we need to move the world away from our camera
  mat4 posTransform = genTranslationMatrix(-cameraPos);
  return posTransform;
}

std::vector<Line3D> A2::clipToUnitCube(std::vector<Line3D> lines) {
  lines = clipNear(lines, -1);
  lines = clipFar(lines, 1);
  lines = clipLeft(lines, -1);
  lines = clipRight(lines, 1);
  lines = clipBottom(lines, -1);
  lines = clipTop(lines, 1);
  return lines;
}

// Add clipping
vector<Line3D> A2::clipNear(vector<Line3D> lines, float near) {
  vector<Line3D> clipped;
  for (Line3D line : lines) {
    float A = line.start.z;
    float B = line.end.z;
    if (A < near && B < near)
      continue;
    else if (A >= near && B >= near)
      clipped.push_back(line);
    else {
      float t = (near - A) / (B - A);
      vec4 AtoB = line.end - line.start;
      if (A > near) // A is valid
      {
        Line3D newLine =
            genLine3D(line.start, line.start + t * AtoB, line.color);
        clipped.push_back(newLine);
      } else {
        Line3D newLine = genLine3D(line.start + t * AtoB, line.end, line.color);
        clipped.push_back(newLine);
      }
    }
  }
  return clipped;
}
vector<Line3D> A2::clipFar(vector<Line3D> lines, float far) {
  vector<Line3D> clipped;
  for (Line3D line : lines) {
    float A = line.start.z;
    float B = line.end.z;
    if (A > far && B > far)
      continue;
    else if (A <= far && B <= far)
      clipped.push_back(line);
    else {
      float t = (far - A) / (B - A);
      vec4 AtoB = line.end - line.start;
      if (A <= far) {
        Line3D newLine =
            genLine3D(line.start, line.start + t * AtoB, line.color);
        clipped.push_back(newLine);
      } else {
        Line3D newLine = genLine3D(line.start + t * AtoB, line.end, line.color);
        clipped.push_back(newLine);
      }
    }
  }
  return clipped;
}
vector<Line3D> A2::clipLeft(vector<Line3D> lines, float left) {
  vector<Line3D> clipped;
  for (Line3D line : lines) {
    float A = line.start.x;
    float B = line.end.x;
    if (A < left && B < left)
      continue;
    else if (A >= left && B >= left)
      clipped.push_back(line);
    else {
      float t = (left - A) / (B - A);
      vec4 AtoB = line.end - line.start;
      if (A >= left) {
        Line3D newLine =
            genLine3D(line.start, line.start + t * AtoB, line.color);
        clipped.push_back(newLine);
      } else {
        Line3D newLine = genLine3D(line.start + t * AtoB, line.end, line.color);
        clipped.push_back(newLine);
      }
    }
  }
  return clipped;
}
vector<Line3D> A2::clipRight(vector<Line3D> lines, float right) {
  vector<Line3D> clipped;
  for (Line3D line : lines) {
    float A = line.start.x;
    float B = line.end.x;
    if (A > right && B > right)
      continue;
    else if (A <= right && B <= right)
      clipped.push_back(line);
    else {
      float t = (right - A) / (B - A);
      vec4 AtoB = line.end - line.start;
      if (A <= right) {
        Line3D newLine =
            genLine3D(line.start, line.start + t * AtoB, line.color);
        clipped.push_back(newLine);
      } else {
        Line3D newLine = genLine3D(line.start + t * AtoB, line.end, line.color);
        clipped.push_back(newLine);
      }
    }
  }
  return clipped;
}
vector<Line3D> A2::clipTop(vector<Line3D> lines, float top) {
  vector<Line3D> clipped;
  for (Line3D line : lines) {
    float A = line.start.y;
    float B = line.end.y;
    if (A > top && B > top)
      continue;
    else if (A <= top && B <= top)
      clipped.push_back(line);
    else {
      float t = (top - A) / (B - A);
      vec4 AtoB = line.end - line.start;
      if (A <= top) {
        Line3D newLine =
            genLine3D(line.start, line.start + t * AtoB, line.color);
        clipped.push_back(newLine);
      } else {
        Line3D newLine = genLine3D(line.start + t * AtoB, line.end, line.color);
        clipped.push_back(newLine);
      }
    }
  }
  return clipped;
}
vector<Line3D> A2::clipBottom(vector<Line3D> lines, float bottom) {
  vector<Line3D> clipped;
  for (Line3D line : lines) {
    float A = line.start.y;
    float B = line.end.y;
    if (A < bottom && B < bottom)
      continue;
    else if (A >= bottom && B >= bottom)
      clipped.push_back(line);
    else {
      float t = (bottom - A) / (B - A);
      vec4 AtoB = line.end - line.start;
      if (A >= bottom) {
        Line3D newLine =
            genLine3D(line.start, line.start + t * AtoB, line.color);
        clipped.push_back(newLine);
      } else {
        Line3D newLine = genLine3D(line.start + t * AtoB, line.end, line.color);
        clipped.push_back(newLine);
      }
    }
  }
  return clipped;
}

mat4 A2::genViewMatrix(vec3 eyePos, vec3 objPos, vec3 up) {
  // Calculate orientation of the camera
  vec3 camForward = normalize(vec3(objPos - eyePos));
  vec3 camRight = normalize(cross(camForward, up));
  vec3 camUp = normalize(cross(camForward, camRight));
  mat4 mat =
      mat4(camRight.x, camRight.y, camRight.z, -dot(camRight, eyePos), camUp.x,
           camUp.y, camUp.z, -dot(camUp, eyePos), -camForward.x, -camForward.y,
           -camForward.z, -dot(camForward, eyePos), 0.0f, 0.0f, 0.0f, 1.0f);
  return transpose(mat);
}

mat4 A2::genProjectionMatrix(float fov, float aspect, float n, float f) {
  float d = 1 / tan(fov / 2.0f);

  mat4 mat =
      mat4(d / aspect, 0.0f, 0.0f, 0.0f, 0.0f, d, 0.0f, 0.0f, 0.0f, 0.0f,
           (f + n) / (f - n), (-2 * f * n) / (f - n), 0.0f, 0.0f, 1.0f, 0.0f);

  return transpose(mat);
}

vector<Line3D> A2::homogenizeLines(vector<Line3D> lines) {
  vector<Line3D> homogenizedLines;
  for (Line3D line : lines) {
    Line3D homogenizedLine;
    homogenizedLine.color = line.color;
    homogenizedLine.start = line.start / line.start.w;
    homogenizedLine.end = line.end / line.end.w;
    homogenizedLines.push_back(homogenizedLine);
  }
  return homogenizedLines;
}

glm::mat4 A2::genViewportMatrix(Viewport viewport) {
  float width = viewport.bottomRight.x - viewport.topLeft.x;
  float height = viewport.topLeft.y - viewport.bottomRight.y;
  float left = viewport.topLeft.x;
  float top = viewport.topLeft.y;
  mat4 mat = mat4(width / 2.0f, 0.0f, 0.0f, left + width / 2.0f, 0.0f,
                  height / 2.0f, 0.0f, -height / 2.0f + top, 0.0f, 0.0f, 1.0f,
                  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  return transpose(mat);
}

mat4 A2::genOrthographicMatrix(vec3 leftbottomnear, vec3 righttopfar) {
  // Move orthographic view to center
  float l = leftbottomnear.x;
  float r = righttopfar.x;
  float b = leftbottomnear.y;
  float t = righttopfar.y;
  float n = leftbottomnear.z;
  float f = righttopfar.z;
  float midx = (r + l) / 2.0f;
  float midy = (t + b) / 2.0f;
  float midz = (f + n) / 2.0f;
  float scalex = 2.0f / (r - l);
  float scaley = 2.0f / (t - b);
  float scalez = 2.0f / (f - n);

  // Translation should be cool?
  mat4 translation = genTranslationMatrix(vec3(-midx, -midy, -midz));
  mat4 scale = genScaleMatrix(vec3(scalex, scaley, scalez));
  return scale * translation;
}

vector<Line3D> A2::transform3DLines(vector<Line3D> lines, glm::mat4 matrix) {
  vector<Line3D> transformedLines;
  for (Line3D line : lines) {
    vec4 transformedStart = matrix * line.start;
    vec4 transformedEnd = matrix * line.end;
    Line3D transformedLine =
        genLine3D(transformedStart, transformedEnd, line.color);
    transformedLines.push_back(transformedLine);
  }
  return transformedLines;
}

void A2::draw3DLineArray(std::vector<Line3D> lines) {
  for (Line3D line : lines) {
    setLineColour(line.color);
    drawLine(vec2(line.start), vec2(line.end));
  }
}

void A2::drawViewport(Viewport viewport) {
  float width = viewport.bottomRight.x - viewport.topLeft.x;
  float height = viewport.topLeft.y - viewport.bottomRight.y;
  vec2 topLeft = viewport.topLeft;
  vec2 topRight = vec2(topLeft.x + width, topLeft.y);
  vec2 botLeft = vec2(topLeft.x, topLeft.y - height);
  vec2 botRight = vec2(topLeft.x + width, topLeft.y - height);

  setLineColour(COLOR_WHITE);
  drawLine(topLeft, topRight);
  drawLine(topRight, botRight);
  drawLine(botRight, botLeft);
  drawLine(botLeft, topLeft);
}

void A2::initControl() {
  interactionMode = ROTATE_MODEL;

  eyePos = vec3(2.0f, 2.0f, 2.0f);
  objPos = vec3(0.0f, 0.0f, 0.0f);
  eyeUp = vec3(0.0f, 1.0f, 0.0f);
  eyeFov = 1.5f;
  eyeNear = 0.1;
  eyeFar = 5;
  eyePitch = eyeRoll = eyePitch = 0.0f;

  modelPos = vec3(0.0f, 0.0f, 0.0f);
  modelScale = vec3(1.0f, 1.0f, 1.0f);
  modelRoll = 0.0f;
  modelPitch = 0.0f;
  modelYaw = 0.0f;

  viewport.topLeft = vec2(-0.9f, 0.9f);
  viewport.bottomRight = vec2(0.9f, -0.9f);

  mouse_left = mouse_middle = mouse_right = false;
  mouse_x = mouse_y = 0;
}
