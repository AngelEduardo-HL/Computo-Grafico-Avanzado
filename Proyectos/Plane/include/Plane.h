#pragma once

#include "glad/glad.h"
#include "glm\glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Plane
{
public:
	GLfloat* normals,
		* plane,
		* textureCoords;

	Plane();
	~Plane();

	glm::vec3 aRotations;
	glm::mat4 mTransform;


	GLint uTransform[2];
	GLint uTime[2];
	GLint uEye[2];
	GLuint texture_id[2];
	GLint uTexture[2];
	GLuint vao;

	size_t size;

	void createPlane(int side = 100);
	void cleanMemory();
	int getNumVertex();

	int side;

	size_t getVertexSizeInBytes();
	size_t getTextureCoordsSizeInBytes();

};