// Termm--Fall 2023

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

#define M_PI 3.14159265358979323846
static const size_t DIM = 16;
static const size_t SLICES = 10;
static const size_t STACKS = 4;
static const size_t SPHERE_VERTICIES = (SLICES + 1) * (STACKS + 1) * 2;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1() : maze(DIM)
{
    current_col = 0;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
    // Initialize random number generator
    int rseed = getpid();
    srandom(rseed);
    // Print random number seed in case we want to rerun with
    // same random numbers
    cout << "Random number seed = " << rseed << endl;

    // Set the background colour.
    glClearColor(0.3, 0.5, 0.7, 1.0);

    // Build the shader
    m_shader.generateProgramObject();
    m_shader.attachVertexShader(
        getAssetFilePath("VertexShader.vs").c_str());
    m_shader.attachFragmentShader(
        getAssetFilePath("FragmentShader.fs").c_str());
    m_shader.link();

    // Set up the uniforms
    P_uni = m_shader.getUniformLocation("P");
    V_uni = m_shader.getUniformLocation("V");
    M_uni = m_shader.getUniformLocation("M");
    col_uni = m_shader.getUniformLocation("colour");

    initGrid();
    initCube();
    initFloor();
    initAvatar();
    camera_rotation = M_PI / 2;
    camera_persistence = 0.0f;
    projection_zoom = 1.0f;
    model_scale = 1.0f;

    // Set up initial view and projection matrices (need to do this here,
    // since it depends on the GLFW window being set up correctly).

    view = glm::lookAt(
        glm::vec3(float(DIM) * 2.0 * M_SQRT1_2 * cos(camera_rotation), 2. * float(DIM) * 2.0 * M_SQRT1_2, float(DIM) * 2.0 * M_SQRT1_2 * sin(camera_rotation)),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    proj = glm::perspective(
        glm::radians(30.0f),
        float(m_framebufferWidth) / float(m_framebufferHeight),
        1.0f, 1000.0f);
}

void A1::initGrid()
{
    size_t sz = 3 * 2 * 2 * (DIM + 3);

    float *verts = new float[sz];
    size_t ct = 0;
    for (int idx = 0; idx < DIM + 3; ++idx)
    {
        verts[ct] = -1;
        verts[ct + 1] = 0;
        verts[ct + 2] = idx - 1;
        verts[ct + 3] = DIM + 1;
        verts[ct + 4] = 0;
        verts[ct + 5] = idx - 1;
        ct += 6;

        verts[ct] = idx - 1;
        verts[ct + 1] = 0;
        verts[ct + 2] = -1;
        verts[ct + 3] = idx - 1;
        verts[ct + 4] = 0;
        verts[ct + 5] = DIM + 1;
        ct += 6;
    }

    // Create the vertex array to record buffer assignments.
    glGenVertexArrays(1, &m_grid_vao);
    glBindVertexArray(m_grid_vao);

    // Create the cube vertex buffer
    glGenBuffers(1, &m_grid_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_grid_vbo);
    glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float),
                 verts, GL_STATIC_DRAW);

    // Specify the means of extracting the position values properly.
    GLint posAttrib = m_shader.getAttribLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Reset state to prevent rogue code from messing with *my*
    // stuff!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // OpenGL has the buffer now, there's no need for us to keep a copy.
    delete[] verts;

    CHECK_GL_ERRORS;
}

void A1::initCube()
{
    float cubeVerticies[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f};

    // Generate VAO
    glGenVertexArrays(1, &m_cube_vao);
    glBindVertexArray(m_cube_vao);

    m_cube_height = 1.0f;
    m_cube_color = vec3(0.5f, 0.0f, 0.0f);
    m_cube_wireframe = false;

    // Generate vertex buffer object to hold cubes verticies
    glGenBuffers(1, &m_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticies), cubeVerticies, GL_STATIC_DRAW);

    GLint posAttrib = m_shader.getAttribLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void A1::initFloor()
{
    float floorVerticies[] = {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f};

    // Generate VAO
    glGenVertexArrays(1, &m_floor_vao);
    glBindVertexArray(m_floor_vao);

    m_floor_color = vec3(0.0f, 0.5f, 0.0f);

    // Generate vertex buffer object to hold cubes verticies
    glGenBuffers(1, &m_floor_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_floor_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVerticies), floorVerticies, GL_STATIC_DRAW);

    GLint posAttrib = m_shader.getAttribLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERRORS;
}

void A1::initAvatar()
{
    float radius = 0.5f;
    float *verts = new float[SPHERE_VERTICIES * 3];

    // I have used a lambda function here as it will only be used once
    auto sphereCord = [](float radius, float theta, float phi)
    {
        return vec3((cos(theta) * sin(phi) + 1) * radius, (cos(phi) + 1) * radius, (sin(theta) * sin(phi) + 1) * radius);
    };

    int index = 0;
    for (int i = 0; i <= STACKS; i++)
    {
        float phi = i * M_PI / STACKS;
        for (int j = 0; j <= SLICES; j++)
        {
            float theta = j * 2 * M_PI / SLICES;

            vec3 coord1 = sphereCord(radius, theta, phi);
            vec3 coord2 = sphereCord(radius, theta, phi + M_PI / STACKS);

            // Add the vertices to the array
            verts[index++] = coord1.x;
            verts[index++] = coord1.y;
            verts[index++] = coord1.z;

            verts[index++] = coord2.x;
            verts[index++] = coord2.y;
            verts[index++] = coord2.z;
        }
    }

    // Generate VAO
    glGenVertexArrays(1, &m_avatar_vao);
    glBindVertexArray(m_avatar_vao);

    m_avatar_color = vec3(0.0f, 0.0f, 0.5f);
    m_avatar_x = -1;
    m_avatar_y = -1;

    // Generate vertex buffer object to hold cubes verticies
    glGenBuffers(1, &m_avatar_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_avatar_vbo);
    glBufferData(GL_ARRAY_BUFFER, SPHERE_VERTICIES * 3 * sizeof(float), verts, GL_STATIC_DRAW);

    GLint posAttrib = m_shader.getAttribLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] verts;
    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
    if (camera_persistence < -0.005f || camera_persistence > 0.005f)
    {
        camera_rotation += camera_persistence;
        camera_persistence *= 0.9f;
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
    // We already know there's only going to be one window, so for
    // simplicity we'll store button states in static local variables.
    // If there was ever a possibility of having multiple instances of
    // A1 running simultaneously, this would break; you'd want to make
    // this into instance fields of A1.
    static bool showTestWindow(false);
    static bool showDebugWindow(true);

    ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
    float opacity(0.5f);

    ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100, 100), opacity, windowFlags);
    if (ImGui::Button("Quit Application"))
    {
        glfwSetWindowShouldClose(m_window, GL_TRUE);
    }
    if (ImGui::Button("Dig Maze"))
    {
        dig();
    }
    if (ImGui::Button("Reset"))
    {
        reset();
    }

    // Eventually you'll create multiple colour widgets with
    // radio buttons.  If you use PushID/PopID to give them all
    // unique IDs, then ImGui will be able to keep them separate.
    // This is unnecessary with a single colour selector and
    // radio button, but I'm leaving it in as an example.

    // Prefixing a widget name with "##" keeps it from being
    // displayed.

    ImGui::PushID(0);
    if (ImGui::RadioButton("Cube Color", &current_col, 0) || current_col == 0)
    {
        ImGui::SameLine();
        ImGui::ColorEdit3("##Colour", (float *)&m_cube_color);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Floor Color", &current_col, 1) || current_col == 1)
    {
        ImGui::SameLine();
        ImGui::ColorEdit3("##Colour", (float *)&m_floor_color);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Avatar Color", &current_col, 2) || current_col == 2)
    {
        ImGui::SameLine();
        ImGui::ColorEdit3("##Colour", (float *)&m_avatar_color);
    }
    ImGui::PopID();

    ImGui::Checkbox("Wireframe Walls", &m_cube_wireframe);

    ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

    ImGui::End();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
    // Create a transformation to be used on the model.
    mat4 W;

    // Create view matrix based on camera rotation, and scaling
    view = glm::lookAt(
        glm::vec3(float(DIM) * 2.0 * M_SQRT1_2 * cos(camera_rotation), 2. * float(DIM) * 2.0 * M_SQRT1_2, float(DIM) * 2.0 * M_SQRT1_2 * sin(camera_rotation)),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::scale(view, vec3(model_scale, model_scale, model_scale));

    // Create perspective matrix
    proj = glm::perspective(
        glm::radians(30.0f),
        float(m_framebufferWidth) / float(m_framebufferHeight),
        1.0f, 1000.0f);

    m_shader.enable();
    glEnable(GL_DEPTH_TEST);

    // Apply our view and perspective matricies
    glUniformMatrix4fv(P_uni, 1, GL_FALSE, value_ptr(proj));
    glUniformMatrix4fv(V_uni, 1, GL_FALSE, value_ptr(view));

    // Just draw the grid for now.
    glBindVertexArray(m_grid_vao);
    glUniform3f(col_uni, 1, 1, 1);
    W = mat4(1.0f);
    W = glm::translate(W, vec3(-float(DIM) / 2.0f, 0, -float(DIM) / 2.0f));
    glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));
    glDrawArrays(GL_LINES, 0, (3 + DIM) * 4);

    // Draw the cubes
    glBindVertexArray(m_cube_vao);
    glUniform3f(col_uni, m_cube_color.r, m_cube_color.g, m_cube_color.b);
    for (int x = 0; x < DIM; x++)
    {
        for (int y = 0; y < DIM; y++)
        {
            if (maze.getValue(x, y) == 1)
            {
                W = mat4(1.0f);
                W = translate(W, glm::vec3(-float(DIM / 2) + float(x), 0.01f, -float(DIM / 2) + float(y)));
                W = scale(W, glm::vec3(1.0f, m_cube_height, 1.0f));
                glUniformMatrix4fv(M_uni, 1, GL_FALSE, glm::value_ptr(W));
                if (!m_cube_wireframe)
                    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
                else
                    glDrawArrays(GL_LINES, 0, 12 * 3);
            }
        }
    }

    // Draw the floor
    glBindVertexArray(m_floor_vao);
    glUniform3f(col_uni, m_floor_color.r, m_floor_color.g, m_floor_color.b);
    W = mat4(1.0f);
    W = translate(W, vec3(-float(DIM / 2), -0.1f, -float(DIM / 2)));
    W = scale(W, vec3(float(DIM), 1.0f, float(DIM)));
    glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Draw the avatar
    glBindVertexArray(m_avatar_vao);
    glUniform3f(col_uni, m_avatar_color.r, m_avatar_color.g, m_avatar_color.b);
    W = mat4(1.0f);
    W = translate(W, vec3(-float(DIM / 2) + float(m_avatar_x), 0.1f, -float(DIM / 2) + float(m_avatar_y)));
    glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, SPHERE_VERTICIES);

    // Highlight the active square.

    m_shader.disable();

    // Restore defaults
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent(int entered)
{
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos)
{
    bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow())
    {
        if (mouse_pressed)
        {
            double dx = mouse_x - xPos;
            double dy = mouse_y - yPos;
            camera_persistence += dx / 1000.0f;
            eventHandled = true;
        }
        mouse_x = xPos;
        mouse_y = yPos;
    }
    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods)
{
    bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow())
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (actions == GLFW_PRESS)
            {
                mouse_pressed = true;
            }
            else if (actions == GLFW_RELEASE)
            {
                mouse_pressed = false;
            }
        }
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet)
{
    bool eventHandled(false);

    if (!(yOffSet > 0 && model_scale > 1.3) && !(yOffSet < 0 && model_scale < 0.5))
    {
        model_scale += yOffSet / 20;
    }
    eventHandled = true;

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height)
{
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods)
{
    bool eventHandled(false);

    // Fill in with event handling code...
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_SPACE)
        {
            m_cube_height += 1.0f;
            eventHandled = true;
        }
        if (key == GLFW_KEY_BACKSPACE)
        {
            if (m_cube_height >= 1.0f)
            {
                m_cube_height -= 1.0f;
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_UP)
        {
            if (validMove(m_avatar_x, m_avatar_y - 1))
            {
                m_avatar_y -= 1;
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_DOWN)
        {
            if (validMove(m_avatar_x, m_avatar_y + 1))
            {
                m_avatar_y += 1;
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_LEFT)
        {
            if (validMove(m_avatar_x - 1, m_avatar_y))
            {
                m_avatar_x -= 1;
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_RIGHT)
        {
            if (validMove(m_avatar_x + 1, m_avatar_y))
            {
                m_avatar_x += 1;
            }
            eventHandled = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_D)
        {
            dig();
            eventHandled = true;
        }
        if (key == GLFW_KEY_R)
        {
            reset();
            eventHandled = true;
        }
        if (key == GLFW_KEY_Q)
        {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
            eventHandled = true;
        }
        if (key == GLFW_KEY_LEFT_SHIFT)
        {
            shift_pressed = true;
            eventHandled = true;
        }
    }
    if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_LEFT_SHIFT)
        {
            shift_pressed = false;
            eventHandled = true;
        }
    }
    return eventHandled;
}

bool A1::validMove(int x, int y)
{
    if (inMazeRange(x, y))
    {
        if (shift_pressed)
        {
            maze.setValue(x, y, 0);
            return true;
        }
        return maze.getValue(x, y) == 0;
    }
    else if (x < -1 || y < -1 || x > int(DIM) || y > int(DIM))
    {
        return false;
    }
    return true;
}

bool A1::inMazeRange(int x, int y)
{
    return (0 <= x && x < DIM && 0 <= y && y < DIM);
}

void A1::dig()
{
    maze.digMaze();
    m_avatar_x = -1;
    m_avatar_y = -1;
}
void A1::reset()
{
    maze.reset();
    m_avatar_x = -1;
    m_avatar_y = -1;
    m_cube_color = vec3(0.5f, 0.0f, 0.0f);
    m_floor_color = vec3(0.0f, 0.5f, 0.0f);
    m_avatar_color = vec3(0.0f, 0.0f, 0.5f);
    camera_rotation = M_PI / 2;
    camera_persistence = 0.0f;
}