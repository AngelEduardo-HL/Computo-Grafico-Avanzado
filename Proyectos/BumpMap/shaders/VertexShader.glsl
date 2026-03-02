#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vTexCoord;

uniform mat4 camera;
uniform mat4 projection;
uniform mat4 model;

out VERTEX_OUT
{
    vec3 posVS;          // posición en View Space
    vec2 uv;             // coordenadas UV
    mat3 TangentView;   // matriz de tangente en View Space
} vertex_out;

void main()
{
    // Posicion en View Space
    vec4 posVS4 = camera * model * vPosition;
    vertex_out.posVS = posVS4.xyz;

    // Pasar UV
    vertex_out.uv = vTexCoord;

    //Base del plano
    vec3 Normal_Obj = vec3(0.0, 1.0, 0.0); 
    vec3 TangentX   = vec3(1.0, 0.0, 0.0);
    vec3 TangentZ   = vec3(0.0, 0.0, 1.0);

    // Transformación hacia View Space
    mat3 M = mat3(camera * model);

    vec3 TangentX_vs = normalize(M * TangentX);
    vec3 TangentZ_vs = normalize(M * TangentZ);

    // Normal matrix para normales
    mat3 normalMat = transpose(inverse(M));
    vec3 NormalVS  = normalize(normalMat * Normal_Obj);

    TangentX_vs = normalize(TangentX_vs - dot(TangentX_vs, NormalVS) * NormalVS);
    TangentZ_vs = normalize(cross(NormalVS, TangentX_vs));

    vertex_out.TangentView = mat3(TangentX_vs, TangentZ_vs, NormalVS);

    gl_Position = projection * posVS4;
}