#include "Application.h"
#include <iostream>
#include "ShaderFuncs.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


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

	glBufferData(GL_ARRAY_BUFFER, oPlane.getVertexSizeInBytes() +
		oPlane.getTextureCoordsSizeInBytes(), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, oPlane.getVertexSizeInBytes(), oPlane.plane);

	glBufferSubData(GL_ARRAY_BUFFER, oPlane.getVertexSizeInBytes(),
		oPlane.getTextureCoordsSizeInBytes(), oPlane.textureCoords);

	oPlane.cleanMemory();

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(oPlane.getVertexSizeInBytes()));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnable(GL_DEPTH_TEST);

	//ids["triangle"] = VAO;
}

void Application::setupProgram()
{
	std::string vertexShader = loadTextFile("shaders/VertexCamera.glsl");
	std::string fragmentShader = loadTextFile("shaders/FragmentCamera.glsl");
	ids["program"] = InitializeProgram(vertexShader, fragmentShader);
	ids["time"] = glGetUniformLocation(ids["program"], "time");
	ids["camera"] = glGetUniformLocation(ids["program"], "camera");
	ids["projection"] = glGetUniformLocation(ids["program"], "projection");
	ids["amplitude"] = glGetUniformLocation(ids["program"], "amplitude");
	ids["phase"] = glGetUniformLocation(ids["program"], "phase");
	ids["frecuency"] = glGetUniformLocation(ids["program"], "frecuency");
}

GLuint Application::setupTexture(const std::string& path)
{
	int width, height, channels;
	unsigned char* img = stbi_load(path.c_str(), &width, &height, &channels, 4);
	if (img == nullptr)
	{
		std::cerr << "Error loading texture: " << path << std::endl;
		return -1;
	}

	GLuint texID = -1;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);

	stbi_image_free(img);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}

void Application::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//teclas para mover	
}

void Application::setup()
{
	setupGeometry();
	setupProgram();
	ids["jeff"] = setupTexture("Textures/Epstein.png");
	projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);
}

void Application::update()
{
	time += 0.1f;
	eye = glm::vec3(0.0f, 2.0f, 2.0f);
	camera = glm::lookAt(eye, center, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Application::draw()
{
	//Seleccionar programa (shaders)
	glUseProgram(ids["program"]);

	//Pasar el resto de los parámetros para el programa
	glUniform1f(ids["time"], time);
	glUniformMatrix4fv(ids["camera"], 1, GL_FALSE, &camera[0][0]);
	glUniformMatrix4fv(ids["projection"], 1, GL_FALSE, &projection[0][0]);

	glUniform1f(ids["amplitude"], 0.1f);
	glUniform1f(ids["phase"], time);
	glUniform1f(ids["frecuency"], 9.0f);

	//Seleccionar la geometria (el triangulo)
	glBindVertexArray(oPlane.vao);

	//glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	glPolygonMode(GL_FRONT, GL_LINE);

	//glDraw()
	int numVertices = oPlane.getNumVertex();
	glDrawArrays(GL_TRIANGLES, 0, oPlane.getNumVertex());
}