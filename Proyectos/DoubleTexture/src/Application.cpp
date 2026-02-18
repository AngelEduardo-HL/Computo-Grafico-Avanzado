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
	std::string vertexShader = loadTextFile("shaders/VertexTexture.glsl");
	std::string fragmentShader = loadTextFile("shaders/FragmentTexture.glsl");
	ids["program"] = InitializeProgram(vertexShader, fragmentShader);
	//ids["time"] = glGetUniformLocation(ids["program"], "time");
	ids["model"] = glGetUniformLocation(ids["program"], "model");
	ids["camera"] = glGetUniformLocation(ids["program"], "camera");
	ids["projection"] = glGetUniformLocation(ids["program"], "projection");
	ids["texture"] = glGetUniformLocation(ids["program"], "texture0");
	ids["texture1"] = glGetUniformLocation(ids["program"], "texture1");
	ids["mixValue"] = glGetUniformLocation(ids["program"], "mixValue");
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}

//Ejercicio: Cargar dos imagenes y que conforme el scroll del mouse se vaya poco a poco mostrando una u otra

void Application::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//teclas para mover	
}

void Application::scrollCallback(double xoffset, double yoffset)
{
	if (yoffset > 0)
	{
		std::cout << "Yoffset: " << yoffset << std::endl;
		mixValue += 0.01f;
		mixValue = std::min(mixValue, 1.0f);
		std::cout << "Mix Value: " << mixValue << std::endl;
	}
	if (yoffset < 1)
	{
		std::cout << "Yoffset: " << yoffset << std::endl;
		mixValue -= 0.01f;
		mixValue = std::max(mixValue, 0.0f);
		std::cout << "Mix Value: " << mixValue << std::endl;
	}
}

void Application::setup()
{
	setupGeometry();
	setupProgram();
	ids["jeff"] = setupTexture("Textures/Epstein.png");
	ids["dora"] = setupTexture("Textures/Dora.png");
}

void Application::update()
{
	time += 0.01f;

	eye = glm::vec3(0.0f, 2.0f + cos(time), 2.0f + cos(time));
	center = glm::vec3(0.01f, 0.01f, 0.01f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	camera = glm::lookAt(eye, center, up);

	model = glm::identity<glm::mat4>();
	projection = glm::perspective(glm::radians(45.0f), (1024.0f / 768.0f), 0.1f, 100.0f);
}

void Application::draw()
{
	//Seleccionar programa (shaders)
	glUseProgram(ids["program"]);

	//Pasar el resto de los parámetros para el programa
	//glUniform1f(ids["time"], time);
	glUniformMatrix4fv(ids["model"], 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(ids["camera"], 1, GL_FALSE, &camera[0][0]);
	glUniformMatrix4fv(ids["projection"], 1, GL_FALSE, &projection[0][0]);
	glUniform1f(ids["mixValue"], mixValue);

	//Seleccionar la geometria (el triangulo)
	glBindVertexArray(oPlane.vao);

	//Textura 1
	glBindTexture(GL_TEXTURE_2D, ids["jeff"]);
	glUniform1i(ids["texture"], 0);
	glActiveTexture(GL_TEXTURE0);

	//Textura 2
	glBindTexture(GL_TEXTURE_2D, ids["dora"]);
	glUniform1i(ids["texture1"], 1);
	glActiveTexture(GL_TEXTURE1);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//glDraw()
	int numVertices = oPlane.getNumVertex();
	glDrawArrays(GL_TRIANGLES, 0, oPlane.getNumVertex());
}