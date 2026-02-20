#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;

// HeightMap
uniform sampler2D heightMap;
uniform float heightScale;

out vec2 texCoord;

void main ()
{
    texCoord = vTexCoord;

    vec3 h = texture(heightMap, vTexCoord).rgb;
    //Calcula la Luminosidad del MierdiMapa
    float L = dot(h, vec3(0.2126, 0.7152, 0.0722)); // 0=negro, 1=blanco
    //float L = dot(h, vec3(0.299, 0.587, 0.114)); // 0=negro, 1=blanco

    vec4 displaced = vPosition;
    displaced.y += L * heightScale; //La altura por ser Blanco y no Negro

    gl_Position = projection * camera * model * displaced;
}