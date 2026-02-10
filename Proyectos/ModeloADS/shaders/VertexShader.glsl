#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 camera;
uniform mat4 projection;
uniform mat4 model;

struct Light {
    vec3 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    int shininess;
};

uniform Light light;
uniform Material material;

out vec4 outColor;

vec4 Ambient(Light l, Material m)
{
    return l.ambient * m.ambient;
}

vec4 Diffuse(Light l, Material m, vec3 N, vec3 Ldir)
{
    float ndotl = max(dot(N, Ldir), 0.0);
    return l.diffuse * m.diffuse * ndotl;
}

vec4 Specular(Light l, Material m, vec3 N, vec3 Ldir, vec3 Vdir)
{
    vec3 R = reflect(-Ldir, N);// direccion de reflejo
    float spec = pow(max(dot(Vdir, R), 0.0), float(m.shininess));
    return l.specular * m.specular * spec;
}

vec4 ADS(Light l, Material m, vec3 N, vec3 Ldir, vec3 Vdir)
{
    return Ambient(l, m) + Diffuse(l, m, N, Ldir) + Specular(l, m, N, Ldir, Vdir);
}

void main()
{
    mat4 accum = model; // matriz del modelo
    vec4 posVS = camera * accum * vPosition; // Pos en el Espacio (Se escribe al reves)

    mat4 matForNormal = transpose(inverse(camera * accum));
    vec3 normalObj = vec3(0.0, 1.0, 0.0); // normal del plano
    vec3 newNormal = normalize((matForNormal * vec4(normalObj, 0.0)).xyz);

    vec3 lightPosVS = (camera * vec4(light.position, 1.0)).xyz; // posicion de la luz en el espacio de vista
    vec3 Ldir = normalize(lightPosVS - posVS.xyz); // direccion de la luz (desde el punto a la luz)

    vec3 Vdir = normalize(-posVS.xyz);

    outColor = ADS(light, material, newNormal, Ldir, Vdir);

    gl_Position = projection * posVS;
}
