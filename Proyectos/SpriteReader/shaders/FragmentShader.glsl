#version 460 core

in vec4 vertexColor;
out vec4 outColor;
uniform float time;

void main()
{
	outColor = vertexColor;
}