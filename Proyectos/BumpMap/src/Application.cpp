#include "Application.h"
#include <iostream>
#include "ShaderFuncs.h"
#include "glm/gtc/type_ptr.hpp"
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
}

void Application::setupProgram()
{
	std::string vertexShader = loadTextFile("shaders/VertexShader.glsl");
	std::string fragmentShader = loadTextFile("shaders/FragmentShader.glsl");
	ids["program"] = InitializeProgram(vertexShader, fragmentShader);

	// Matrices
	ids["camera"] = glGetUniformLocation(ids["program"], "camera");
	ids["projection"] = glGetUniformLocation(ids["program"], "projection");
	ids["model"] = glGetUniformLocation(ids["program"], "model");

	// Light struct uniforms
	ids["light.position"] = glGetUniformLocation(ids["program"], "light.position");
	ids["light.ambient"] = glGetUniformLocation(ids["program"], "light.ambient");
	ids["light.diffuse"] = glGetUniformLocation(ids["program"], "light.diffuse");
	ids["light.specular"] = glGetUniformLocation(ids["program"], "light.specular");

	// Material struct uniforms
	ids["material.ambient"] = glGetUniformLocation(ids["program"], "material.ambient");
	ids["material.diffuse"] = glGetUniformLocation(ids["program"], "material.diffuse");
	ids["material.specular"] = glGetUniformLocation(ids["program"], "material.specular");
	ids["material.shininess"] = glGetUniformLocation(ids["program"], "material.shininess");

	//Texture
	ids["colorMap"] = glGetUniformLocation(ids["program"], "colorMap");
	ids["normalMap"] = glGetUniformLocation(ids["program"], "normalMap");
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

	//teclas para mover	
}

void Application::scrollCallback(double xoffset, double yoffset)
{
	if (yoffset > 0)
	{
		eye += 0.5f * (center - eye);
	}
	else if (yoffset < 0)
	{
		eye -= 0.5f * (center - eye);
	}
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

	// Luz
	light.position = glm::vec3(0.0f, 6.0f, 0.0f);
	light.ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	light.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Material
	material.ambient = glm::vec4(0.20f, 0.00f, 0.00f, 1.0f);
	material.diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	material.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	material.shininess = 16;

	projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);

	// Matriz del modelo
	model = glm::mat4(1.0f);

	// Texturas
	texColorID = setupTexture("Textures/Rocks_Color.png");
	texNormalID = setupTexture("Textures/Rocks_Normal.png");
}

void Application::update()
{
	time += 0.01f;

	// Camara
	eye = glm::vec3(0.0f, 2.0f, 2.0f);
	camera = glm::lookAt(eye, center, glm::vec3(0.0f, 1.0f, 0.0f));

	// Rotacion del plano
	//model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	light.position = glm::vec3(6.0f * cos(time), 6.0f, 6.0f * sin(time));
}

void Application::draw()
{
	glUseProgram(ids["program"]);

	// Matrices
	glUniformMatrix4fv(ids["camera"], 1, GL_FALSE, glm::value_ptr(camera));
	glUniformMatrix4fv(ids["projection"], 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(ids["model"], 1, GL_FALSE, glm::value_ptr(model));

	// Luz
	glUniform3fv(ids["light.position"], 1, glm::value_ptr(light.position));
	glUniform4fv(ids["light.ambient"], 1, glm::value_ptr(light.ambient));
	glUniform4fv(ids["light.diffuse"], 1, glm::value_ptr(light.diffuse));
	glUniform4fv(ids["light.specular"], 1, glm::value_ptr(light.specular));

	// Material
	glUniform4fv(ids["material.ambient"], 1, glm::value_ptr(material.ambient));
	glUniform4fv(ids["material.diffuse"], 1, glm::value_ptr(material.diffuse));
	glUniform4fv(ids["material.specular"], 1, glm::value_ptr(material.specular));
	glUniform1i(ids["material.shininess"], material.shininess);

	glBindVertexArray(oPlane.vao);

	//Texturas
	glUniform1i(ids["colorMap"], 0);
	glUniform1i(ids["normalMap"], 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texColorID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texNormalID);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, oPlane.getNumVertex());
}