#pragma once
#include <vector>
#include <map>
#include "glad/glad.h"
#include <string>
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Plane.h"

class Application
{
private:

	std::map<std::string, GLuint> ids;
	Plane oPlane;

	void setupGeometry();
	void setupProgram();
	GLuint setupTexture(const std::string& path);

	// --- SpriteSheet ---
	GLuint spriteTex{ 0 };
	int sheetW{ 0 }, sheetH{ 0 };
	const int frameW{ 35 };
	const int frameH{ 55 };
	int cols{ 1 }, rows{ 1 };
	int frame{ 0 };
	int currentRow{ 0 };
	float animTimer{ 0.0f };
	float animFPS{ 12.0f };

	bool keyA{ false }, keyD{ false }, keyShift{ false };
	bool stepA{ false }, stepD{ false };

	float time{ 0.0f };
	float mixValue{ 0.0f };

	glm::mat4 model;
	glm::mat4 camera;
	glm::mat4 projection;

	glm::vec3 eye{ 0.0f, 0.0f, 2.0f };
	glm::vec3 center{ 0.1f, 0.1f, 0.5f };

public:
	Application();
	GLFWwindow* window;

	void setup();
	void update();
	void draw();

	void keyCallback(int key, int scancode, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
};