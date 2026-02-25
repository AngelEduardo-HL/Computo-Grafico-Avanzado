#version 460 core

layout (location = 0) in vec4 vPosition;

uniform float time;
uniform mat4 camera;
uniform mat4 projection;

void main()
{
    vec4 newPosition = vPosition;

    // newPosition.y = newPosition.y + cos(time);

    gl_Position = projection * camera * newPosition;
}
