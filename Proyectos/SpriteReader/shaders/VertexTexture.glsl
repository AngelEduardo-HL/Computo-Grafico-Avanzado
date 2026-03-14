#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;

uniform vec2 texOffset; // Es el Ddesplazamiento de textura
uniform vec2 texScale;

out vec2 texCoord;

void main ()
{
    texCoord = texOffset + (vTexCoord * texScale); 
    gl_Position = projection * camera * model * vPosition;
}