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
	oPlane.createPlane(1);

	glGenVertexArrays(1, &oPlane.vao);
	glBindVertexArray(oPlane.vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER,
		oPlane.getVertexSizeInBytes() + oPlane.getTextureCoordsSizeInBytes(),
		NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, oPlane.getVertexSizeInBytes(), oPlane.plane);

	glBufferSubData(GL_ARRAY_BUFFER, oPlane.getVertexSizeInBytes(),
		oPlane.getTextureCoordsSizeInBytes(), oPlane.textureCoords);

	oPlane.cleanMemory();

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(oPlane.getVertexSizeInBytes()));

	glEnableVertexAttribArray(0);
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

	ids["texture"] = glGetUniformLocation(ids["program"], "texture0");
	ids["frameOffset"] = glGetUniformLocation(ids["program"], "uFrameOffset");
	ids["frameScale"] = glGetUniformLocation(ids["program"], "uFrameScale");
}

GLuint Application::setupTexture(const std::string& path)
{
	int width, height, channels;

	stbi_set_flip_vertically_on_load(true);

	unsigned char* img = stbi_load(path.c_str(), &width, &height, &channels, 4);
	if (img == nullptr)
	{
		std::cerr << "Error loading texture: " << path << std::endl;
		return (GLuint)-1;
	}

	GLuint texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, img);

	stbi_image_free(img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void Application::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// --- SpriteSheet input ---
	if (key == GLFW_KEY_A)
	{
		if (action == GLFW_PRESS) { keyA = true; stepA = true; }
		if (action == GLFW_RELEASE) { keyA = false; }
	}

	if (key == GLFW_KEY_D)
	{
		if (action == GLFW_PRESS) { keyD = true; stepD = true; }
		if (action == GLFW_RELEASE) { keyD = false; }
	}

	if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT)
	{
		if (action == GLFW_PRESS) { keyShift = true; }
		if (action == GLFW_RELEASE) { keyShift = false; }
	}
}

void Application::scrollCallback(double xoffset, double yoffset)
{
}

void Application::setup()
{
	setupGeometry();
	setupProgram();

	spriteTex = setupTexture("Textures/SpriteSheet.png");

	if (spriteTex == (GLuint)-1)
		spriteTex = setupTexture("SpriteSheet.png");

	int w, h, c;
	unsigned char* tmp = stbi_load("Textures/SpriteSheet.png", &w, &h, &c, 4);
	if (!tmp) tmp = stbi_load("SpriteSheet.png", &w, &h, &c, 4);

	if (tmp)
	{
		sheetW = w; sheetH = h;
		stbi_image_free(tmp);
	}

	cols = std::max(1, sheetW / frameW);
	rows = std::max(1, sheetH / frameH);
}

void Application::update()
{
	float dt = 1.0f / 60.0f;
	time += dt;

	eye = glm::vec3(0.0f, 2.0f + cos(time), 2.0f + cos(time));
	center = glm::vec3(0.01f, 0.01f, 0.01f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	camera = glm::lookAt(eye, center, up);

	model = glm::identity<glm::mat4>();
	projection = glm::perspective(glm::radians(45.0f), (1024.0f / 768.0f), 0.1f, 100.0f);

	// --- SpriteSheet logic ---
	if (keyD && !keyA) currentRow = 0;
	if (keyA && !keyD) currentRow = 1;

	bool play = keyShift && (keyA || keyD);
	if (play)
	{
		animTimer += dt;
		float spf = 1.0f / std::max(1.0f, animFPS);

		if (animTimer >= spf)
		{
			animTimer = 0.0f;
			frame = (frame + 1) % cols;
		}
	}

	if (!keyShift)
	{
		if (stepD) { currentRow = 0; frame = (frame + 1) % cols; stepD = false; }
		if (stepA) { currentRow = 1; frame = (frame + 1) % cols; stepA = false; }
	}
}

void Application::draw()
{
	glUseProgram(ids["program"]);

	glUniformMatrix4fv(ids["model"], 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(ids["camera"], 1, GL_FALSE, &camera[0][0]);
	glUniformMatrix4fv(ids["projection"], 1, GL_FALSE, &projection[0][0]);

	glm::vec2 scale(
		(sheetW > 0) ? (float)frameW / (float)sheetW : 1.0f,
		(sheetH > 0) ? (float)frameH / (float)sheetH : 1.0f
	);

	glm::vec2 offset(
		(float)frame * scale.x,
		(float)currentRow * scale.y
	);

	glUniform2f(ids["frameScale"], scale.x, scale.y);
	glUniform2f(ids["frameOffset"], offset.x, offset.y);

	glBindVertexArray(oPlane.vao);

	// SpriteSheet
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spriteTex);
	glUniform1i(ids["texture"], 0);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, oPlane.getNumVertex());
}