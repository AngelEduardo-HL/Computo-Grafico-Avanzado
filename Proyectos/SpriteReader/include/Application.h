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

    void updateState();
    void updateAnimation(float deltaTime);
    void updateSprite();

    float time{ 0.0f };
    float lastFrameTime{ 0.0f };
    float animationTimer{ 0.0f };

    glm::mat4 model;
    glm::mat4 camera;
    glm::mat4 projection;
    glm::vec3 eye{ 0.0f, 2.0f, 2.0f };
    glm::vec3 center{ 0.0f, 0.0f, 0.0f };

    bool keyA{ false };
    bool keyD{ false };
    bool keyShift{ false };

    enum class State
    {
        Idle,
        WalkForward,
        RunForward,
        WalkBackward
    };

    State currentState{ State::Idle };
    State previousState{ State::Idle };

    // Sprite sheet
    const float frameWidthPx = 55.0f;
    const float frameHeightPx = 50.0f;
    const float textureWidthPx = 990.0f;   // 18 * 55
    const float textureHeightPx = 200.0f;  // 4 * 50

    int currentFrame{ 0 };
    int currentRow{ 0 };
    int currentFrameCount{ 8 };

    float fpsIdle{ 8.0f };
    float fpsWalk{ 11.0f };
    float fpsRun{ 18.0f };
    float currentFPS{ 8.0f };

    int framesPerRow[4] = { 8, 16, 16, 16 };

    glm::vec2 texOffset{ 0.0f, 0.0f };
    glm::vec2 texScale{ 1.0f, 1.0f };

public:
    Application();
    GLFWwindow* window{ nullptr };

    void setup();
    void update();
    void draw();
    void keyCallback(int key, int scancode, int action, int mods);
};