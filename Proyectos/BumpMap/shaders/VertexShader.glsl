#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 camera;
uniform mat4 projection;
uniform mat4 model;

out VERTEX_OUT
{
    vec3 posVS;     // posición en View Space
    vec3 normalVS;  // normal en View Space
} vertex_out;

void main()
{
    // Posición en view space
    vec4 posVS4 = camera * model * vPosition;
    vertex_out.posVS = posVS4.xyz;

    vec3 normalObj = vec3(0.0, 1.0, 0.0);

    // Normal Matrix en view space: transpose(inverse(mat3(camera*model)))
    mat3 normalMat = transpose(inverse(mat3(camera * model)));
    vertex_out.normalVS = normalize(normalMat * normalObj);

    gl_Position = projection * posVS4;
}