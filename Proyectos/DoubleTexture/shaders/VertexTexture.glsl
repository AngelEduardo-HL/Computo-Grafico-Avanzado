#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;

out vec2 texCoord;

void main ()
{
	texCoord = vTexCoord;
	gl_Position = projection * camera * model * vPosition;
}