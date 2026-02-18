#version 460 core

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D texture0; // textura 1
uniform sampler2D texture1; // textura 2
uniform float mixValue;

void main ()
{
	vec4 color0 = texture(texture0, texCoord);
	vec4 color1 = texture(texture1, texCoord);
	
	outColor = mix(color0, color1, mixValue);
}