#include "Application.h"
#include <iostream>
#include "ShaderFuncs.h"
#include "glm/gtc/type_ptr.hpp"

Application::Application() : oPlane()
{

}

void Application::setupGeometry()
{
	oPlane.createPlane(3);

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
	ids["material.shininess"] = glGetUniformLocation(ids["program"], "material.shininess"); // NUEVO
}

void Application::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	a
		daw
		faw
		 
		a
		aw
		d
		a
		daw
		d
		ad
	//teclas para mover	
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
}

void Application::update()
{
	time += 0.001f;

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

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glDrawArrays(GL_TRIANGLES, 0, oPlane.getNumVertex());
}