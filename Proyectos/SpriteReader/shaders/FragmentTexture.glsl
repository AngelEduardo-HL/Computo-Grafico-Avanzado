#version 460 core

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D texture0; // The texture sampler for the sprite sheet

void main ()
{
	outColor = texture(texture0, texCoord);
}