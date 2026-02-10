#version 460 core

layout (location = 0) in vec4 vPosition;

uniform float time;
uniform mat4 camera;
uniform mat4 projection;

uniform float amplitude;
uniform float phase;
uniform float frecuency;

float F (float x, float z, float amplitude, float phase, float frecuency)
{
	return amplitude * cos(phase + frecuency * (x * x + z * z));
}

void main ()
{
	vec4 newPosition = vPosition;

	newPosition.y = newPosition.y + F(newPosition.x, newPosition.z, amplitude, phase, frecuency);

	newPosition = projection * camera * newPosition;
	gl_Position = newPosition;
}
