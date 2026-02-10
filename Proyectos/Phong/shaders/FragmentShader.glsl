#version 460 core

in VERTEX_OUT
{
    vec3 posVS;
    vec3 normalVS;
} fragIn; //Interpolacion hacia el Vertex

out vec4 FragColor;

uniform mat4 camera;

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
    int  shininess;
};

uniform Light light;
uniform Material material;

vec4 ADS(Light l, Material m, vec3 N, vec3 Ldir, vec3 Vdir)
{
    // Ambient
    vec4 ambient = l.ambient * m.ambient;

    // Diffuse
    float ndotl = max(dot(N, Ldir), 0.0);
    vec4 diffuse = l.diffuse * m.diffuse * ndotl;

    // Specular
    vec4 specular = vec4(0.0);
    if (ndotl > 0.0)
    {
        vec3 R = reflect(-Ldir, N);
        float spec = pow(max(dot(Vdir, R), 0.0), float(m.shininess));
        specular = l.specular * m.specular * spec;
    }

    return ambient + diffuse + specular;
}

void main()
{
    vec3 N = normalize(fragIn.normalVS);

    vec3 lightPosVS = (camera * vec4(light.position, 1.0)).xyz;

    vec3 Ldir = normalize(lightPosVS - fragIn.posVS);
    vec3 Vdir = normalize(-fragIn.posVS); // cámara en el origen del view space

    FragColor = ADS(light, material, N, Ldir, Vdir);
}