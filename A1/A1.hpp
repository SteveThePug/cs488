// Termm--Fall 2023

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window
{
public:
    A1();
    virtual ~A1();

protected:
    virtual void init() override;
    virtual void appLogic() override;
    virtual void guiLogic() override;
    virtual void draw() override;
    virtual void cleanup() override;

    virtual bool cursorEnterWindowEvent(int entered) override;
    virtual bool mouseMoveEvent(double xPos, double yPos) override;
    virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
    virtual bool windowResizeEvent(int width, int height) override;
    virtual bool keyInputEvent(int key, int action, int mods) override;

private:
    void initGrid();

    // Fields related to the shader and uniforms.
    ShaderProgram m_shader;
    GLint P_uni;   // Uniform location for Projection matrix.
    GLint V_uni;   // Uniform location for View matrix.
    GLint M_uni;   // Uniform location for Model matrix.
    GLint col_uni; // Uniform location for cube colour.

    // Fields related to grid geometry.
    GLuint m_grid_vao;
    GLuint m_grid_vbo;

    // Fields related to cube geometry.
    void initCube();
    GLuint m_cube_vao;
    GLuint m_cube_vbo;
    bool m_cube_wireframe;
    glm::vec3 m_cube_color;
    float m_cube_height;

    // Field related to avatar geometry
    void initAvatar();
    GLuint m_avatar_vao;
    GLuint m_avatar_vbo;
    glm::vec3 m_avatar_color;
    int m_avatar_x, m_avatar_y;

    // Fields related to floor geometry
    void initFloor();
    GLuint m_floor_vao;
    GLuint m_floor_vbo;
    glm::vec3 m_floor_color;

    // Matrices controlling the camera and projection.
    glm::mat4 proj;
    glm::mat4 view;

    // Fields related to view
    float camera_rotation;
    float camera_persistence;
    float model_scale;

    // Field related to projection
    float projection_zoom;

    //  Radio button
    int current_col;

    // Field related to maze game
    Maze maze;
    void dig();
    void reset();
    bool inMazeRange(int x, int y);
    bool validMove(int x, int y);

    // Fields related to input
    bool shift_pressed;
    bool mouse_pressed;
    double mouse_x, mouse_y;
};
