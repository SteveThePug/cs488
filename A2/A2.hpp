// Termm--Fall 2023

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;

// Structs
struct Line3D
{
	glm::vec4 start;
	glm::vec4 end;
	glm::vec3 color;
};
struct Viewport
{
	glm::vec2 topLeft;
	glm::vec2 bottomRight;
};
// Enums
enum InteractionMode
{
	ROTATE_VIEW,
	TRANSLATE_VIEW,
	PERSPECTIVE_VIEW,
	ROTATE_MODEL,
	TRANSLATE_MODEL,
	SCALE_MODEL,
	VIEWPORT
};

// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData
{
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};

class A2 : public CS488Window
{
public:
	A2();
	virtual ~A2();

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

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 &colour);

	void drawLine(
		const glm::vec2 &v0,
		const glm::vec2 &v1);

	ShaderProgram m_shader;

	GLuint m_vao;			// Vertex Array Object
	GLuint m_vbo_positions; // Vertex Buffer Object
	GLuint m_vbo_colours;	// Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	// My Code
	// Color constants
	const glm::vec3 COLOR_WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
	const glm::vec3 COLOR_RED = glm::vec3(1.0f, 0.0f, 0.0f);
	const glm::vec3 COLOR_BLUE = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 COLOR_GREEN = glm::vec3(0.0f, 0.0f, 1.0f);

	// Generate Models
	Line3D genLine3D(glm::vec4 start, glm::vec4 end, glm::vec3 color);
	std::vector<Line3D> generateCubeArray();
	std::vector<Line3D> generateGnomon(glm::vec3 colorX, glm::vec3 colorY, glm::vec3 colorZ);

	// Generate Matricies
	glm::mat4 genIdentityMatrix();
	glm::mat4 genTranslationMatrix(glm::vec3 translation);
	glm::mat4 genRotationMatrix(float rX, float rY, float rZ);
	glm::mat4 genScaleMatrix(glm::vec3 scale);
	glm::mat4 genPositionCamera(glm::vec3 position);

	// Clipping
	// 3D clipping so we can find projection
	std::vector<Line3D> clipToUnitCube(std::vector<Line3D>);
	std::vector<Line3D> clipNear(std::vector<Line3D>, float near);
	std::vector<Line3D> clipFar(std::vector<Line3D>, float far);
	std::vector<Line3D> clipLeft(std::vector<Line3D>, float Left);
	std::vector<Line3D> clipRight(std::vector<Line3D>, float right);
	std::vector<Line3D> clipTop(std::vector<Line3D>, float top);
	std::vector<Line3D> clipBottom(std::vector<Line3D>, float bottom);

	// View Matrix
	glm::mat4 genViewMatrix(glm::vec3 eyePos, glm::vec3 lookAt, glm::vec3 up);

	// Projections and Projection Matricies
	glm::mat4 genOrthographicMatrix(glm::vec3 leftbottomnear, glm::vec3 righttopfar);
	glm::mat4 genProjectionMatrix(float fov, float aspect, float near, float far);
	std::vector<Line3D> homogenizeLines(std::vector<Line3D> lines);

	// Viewport Matrix
	glm::mat4 genViewportMatrix(Viewport viewport);

	// Transforming
	std::vector<Line3D> transform3DLines(std::vector<Line3D> lines, glm::mat4 matrix);

	// Drawing
	void draw3DLineArray(std::vector<Line3D> lines);
	void drawViewport(Viewport viewport);

	// User interaction
	void initControl();
	int interactionMode;
	bool mouse_left, mouse_right, mouse_middle;
	double mouse_x, mouse_y;
	// Eye interaction
	glm::vec3 eyePos;
	glm::vec3 objPos;
	glm::vec3 eyeUp;
	float eyeYaw, eyePitch, eyeRoll;
	float eyeFov, eyeNear, eyeFar;
	// Model interaction
	float modelYaw, modelPitch, modelRoll;
	glm::vec3 modelPos;
	glm::vec3 modelScale;
	// Viewport interaction
	Viewport viewport;

	/*
	Graphics Pipeline:
	1) Apply modelling transformation
		- Rotation, Translation, Scale
	2) Apply view transformation
		- Rotation, Translation
	3) Apply projection transformation
		- Apply projective transformation
	4) Apply clipping
		- Only keep verticies within our projection ([-1,1],[-1,1])
	4) Apply viewport transformation
		- Shrink to fit current viewport

	*/
};
