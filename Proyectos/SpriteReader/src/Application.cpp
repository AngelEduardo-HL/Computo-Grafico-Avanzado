#include "Application.h"
#include <iostream>
#include "ShaderFuncs.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <chrono>
std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

Application::Application() : oPlane()
{
}

void Application::setupGeometry()
{
    oPlane.createPlane(1);

    glGenVertexArrays(1, &oPlane.vao);
    glBindVertexArray(oPlane.vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
        oPlane.getVertexSizeInBytes() + oPlane.getTextureCoordsSizeInBytes(),
        NULL,
        GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0,
        oPlane.getVertexSizeInBytes(), oPlane.plane);

    glBufferSubData(GL_ARRAY_BUFFER, oPlane.getVertexSizeInBytes(),
        oPlane.getTextureCoordsSizeInBytes(), oPlane.textureCoords);

    oPlane.cleanMemory();

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
        (void*)(oPlane.getVertexSizeInBytes()));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Application::setupProgram()
{
    std::string vertexShader = loadTextFile("shaders/VertexTexture.glsl");
    std::string fragmentShader = loadTextFile("shaders/FragmentTexture.glsl");
    ids["program"] = InitializeProgram(vertexShader, fragmentShader);

    ids["model"] = glGetUniformLocation(ids["program"], "model");
    ids["camera"] = glGetUniformLocation(ids["program"], "camera");
    ids["projection"] = glGetUniformLocation(ids["program"], "projection");
    ids["texture"] = glGetUniformLocation(ids["program"], "texture0");

    ids["texOffset"] = glGetUniformLocation(ids["program"], "texOffset");
    ids["texScale"] = glGetUniformLocation(ids["program"], "texScale");
}

GLuint Application::setupTexture(const std::string& path)
{
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* img = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (img == nullptr)
    {
        std::cerr << "Error loading texture: " << path << std::endl;
        return 0;
    }

    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, img);

    stbi_image_free(img);

    // Para pixel art conviene NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

void Application::keyCallback(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    bool released = (action == GLFW_RELEASE);

    if (key == GLFW_KEY_A)
    {
        if (pressed) keyA = true;
        if (released) keyA = false;
    }

    if (key == GLFW_KEY_D)
    {
        if (pressed) keyD = true;
        if (released) keyD = false;
    }

    if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
    {
        if (pressed) keyShift = true;
        if (released) keyShift = false;
    }
}

void Application::setup()
{
    setupGeometry();
    setupProgram();

    ids["Sprite"] = setupTexture("Textures/SpriteSheet.png");

    // Estado inicial: idle
    currentState = State::Idle;
    previousState = State::Idle;
    currentFrame = 0;
    currentRow = 0;
    currentFrameCount = framesPerRow[0];
    currentFPS = fpsIdle;

    updateSprite();
}

void Application::updateState()
{
    previousState = currentState;

    if (keyA)
    {
        if (keyShift)
            currentState = State::RunForward;
        else
            currentState = State::WalkForward;
    }
    else if (keyD)
    {
        currentState = State::WalkBackward;
    }
    else
    {
        currentState = State::Idle;
    }

    if (currentState != previousState)
    {
        currentFrame = 0;
        animationTimer = 0.0f;

        switch (currentState)
        {
        case State::Idle:
            currentRow = 0;
            currentFrameCount = framesPerRow[0];
            currentFPS = fpsIdle;
            break;

        case State::WalkForward:
            currentRow = 1;
            currentFrameCount = framesPerRow[1];
            currentFPS = fpsWalk;
            break;

        case State::RunForward:
            currentRow = 2;
            currentFrameCount = framesPerRow[2];
            currentFPS = fpsRun;
            break;

        case State::WalkBackward:
            currentRow = 3;
            currentFrameCount = framesPerRow[3];
            currentFPS = fpsWalk;
            break;
        }
    }
}

void Application::updateAnimation(float deltaTime)
{
    animationTimer += deltaTime;

    float frameDuration = 1.0f / currentFPS;

    while (animationTimer >= frameDuration)
    {
        animationTimer -= frameDuration;
        currentFrame++;

        if (currentFrame >= currentFrameCount)
            currentFrame = 0;
    }
}

void Application::updateSprite()
{
    texScale.x = frameWidthPx / textureWidthPx; //Saca el ancho del cuadro
	texScale.y = frameHeightPx / textureHeightPx; //Saca el alto del cuadro

	texOffset.x = (currentFrame * frameWidthPx) / textureWidthPx;//Saca el desplazamiento horizontal del cuadro
	texOffset.y = 1.0f - ((currentRow + 1) * frameHeightPx) / textureHeightPx; //Saca el desplazamiento vertical del cuadro
}

void Application::update()
{
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    time += deltaTime;

    updateState();
    updateAnimation(deltaTime);
    updateSprite();

	eye = glm::vec3(0.0f, 3.0f, 2.0f);
    center = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    camera = glm::lookAt(eye, center, up);

    model = glm::identity<glm::mat4>();
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    projection = glm::perspective(
        glm::radians(45.0f),
        (1024.0f / 768.0f),
        0.1f,
        100.0f
    );
}

void Application::draw()
{
    glUseProgram(ids["program"]);

    glUniformMatrix4fv(ids["model"], 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(ids["camera"], 1, GL_FALSE, &camera[0][0]);
    glUniformMatrix4fv(ids["projection"], 1, GL_FALSE, &projection[0][0]);

    glUniform2f(ids["texOffset"], texOffset.x, texOffset.y);
    glUniform2f(ids["texScale"], texScale.x, texScale.y);

    glBindVertexArray(oPlane.vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ids["Sprite"]);
    glUniform1i(ids["texture"], 0);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLES, 0, oPlane.getNumVertex());
}