#include "Application.h"
#include <iostream>
#include "ShaderFuncs.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>

Application::Application() : oPlane()
{
}

void Application::setupGeometry()
{
    oPlane.createPlane(100);

    glGenVertexArrays(1, &oPlane.vao);
    glBindVertexArray(oPlane.vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
        oPlane.getVertexSizeInBytes() + oPlane.getTextureCoordsSizeInBytes(),
        NULL, GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0,
        oPlane.getVertexSizeInBytes(), oPlane.plane);

    glBufferSubData(GL_ARRAY_BUFFER, oPlane.getVertexSizeInBytes(),
        oPlane.getTextureCoordsSizeInBytes(), oPlane.textureCoords);

    oPlane.cleanMemory();

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
        (void*)(oPlane.getVertexSizeInBytes()));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
}

void Application::setupProgram()
{
    std::string vertexShader = loadTextFile("shaders/VertexTexture.glsl");
    std::string fragmentShader = loadTextFile("shaders/FragmentTexture.glsl");
    ids["program"] = InitializeProgram(vertexShader, fragmentShader);

    ids["model"] = glGetUniformLocation(ids["program"], "model");
    ids["camera"] = glGetUniformLocation(ids["program"], "camera");
    ids["projection"] = glGetUniformLocation(ids["program"], "projection");

    // Texturas
    ids["diffuseMap"] = glGetUniformLocation(ids["program"], "diffuseMap");
    ids["heightMap"] = glGetUniformLocation(ids["program"], "heightMap");

    // Height scale
    ids["heightScale"] = glGetUniformLocation(ids["program"], "heightScale");
}

GLuint Application::setupTexture(const std::string& path)
{
    int width, height, channels;
    unsigned char* img = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (img == nullptr)
    {
        std::cerr << "Error loading texture: " << path << std::endl;
        return 0;
    }

    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, img);

    stbi_image_free(img);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

void Application::keyCallback(int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Application::scrollCallback(double xoffset, double yoffset)
{
    if (yoffset > 0)
        heightScale = std::min(heightScale + 0.05f, 3.0f);
    else if (yoffset < 0)
        heightScale = std::max(heightScale - 0.05f, 0.0f);

    std::cout << "heightScale: " << heightScale << std::endl;
}

void Application::mouseButtonCallback(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            dragging = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE)
        {
            dragging = false;
        }
    }
}

void Application::cursorPosCallback(double xpos, double ypos)
{
    if (!dragging) return;

    double dx = xpos - lastX;
    double dy = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    // Sensibilidad
    rotY += (float)(dx * 0.005);
    rotX += (float)(dy * 0.005);

    // Limitar inclinación
	rotX = std::min(rotX, 1.5f);
	rotX = std::max(rotX, -1.5f);
}

void Application::setup()
{
    setupGeometry();
    setupProgram();

    ids["diffuseTex"] = setupTexture("Textures/MountainDeffuse.png");
    ids["heightTex"] = setupTexture("Textures/MountainHeightMap.png");
}

void Application::update()
{
    // Camara
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    camera = glm::lookAt(eye, center, up);

    // Rotación del plano con el mouse
    model = glm::mat4(1.0f);
    model = glm::rotate(model, rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotY, glm::vec3(0.0f, 1.0f, 0.0f));

    projection = glm::perspective(glm::radians(45.0f), (1024.0f / 768.0f), 0.1f, 100.0f);
}

void Application::draw()
{
    glUseProgram(ids["program"]);

    glUniformMatrix4fv(ids["model"], 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(ids["camera"], 1, GL_FALSE, &camera[0][0]);
    glUniformMatrix4fv(ids["projection"], 1, GL_FALSE, &projection[0][0]);

    glUniform1f(ids["heightScale"], heightScale);

    glBindVertexArray(oPlane.vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ids["diffuseTex"]);
    glUniform1i(ids["diffuseMap"], 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ids["heightTex"]);
    glUniform1i(ids["heightMap"], 1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_TRIANGLES, 0, oPlane.getNumVertex());
}