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

    // Transforms
    glm::mat4 model{};
    glm::mat4 camera{};
    glm::mat4 projection{};

    // Camera
    glm::vec3 eye{ 0.0f, 1.6f, 2.6f };
    glm::vec3 center{ 0.0f, 0.0f, 0.0f };

    // Mouse rotation
    bool dragging{ false };
    double lastX{ 0.0 }, lastY{ 0.0 };
    float rotX{ -0.7f };
    float rotY{ 0.6f };

    // Height scale
    float heightScale{ 0.8f };

public:
    Application();
    GLFWwindow* window{ nullptr };

    void setup();
    void update();
    void draw();

    void keyCallback(int key, int scancode, int action, int mods);

    void scrollCallback(double xoffset, double yoffset);

    void mouseButtonCallback(int button, int action, int mods);
    void cursorPosCallback(double xpos, double ypos);
};