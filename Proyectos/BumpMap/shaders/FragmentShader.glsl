#version 460 core

in VERTEX_OUT
{
    vec3 posVS;
    vec2 uv;
    mat3 TangentView;
} fragIn;

out vec4 FragColor;

uniform mat4 camera;

uniform sampler2D colorMap;
uniform sampler2D normalMap;

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

vec4 ADS_withTexture(Light l, Material m, vec3 N, vec3 Ldir, vec3 Vdir, vec4 texColor)
{
    vec4 ambient = l.ambient * texColor;

    float ndotl = max(dot(N, Ldir), 0.0);
    vec4 diffuse = l.diffuse * texColor * ndotl;

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
FragColor = vec4(1, 0, 1, 1);

    //Color del texel desde normalMap
    vec3 nColor = texture(normalMap, fragIn.uv).rgb;

    //Convertir a normal en Tangent Space y normalizar
    vec3 nTangentSpace = normalize(nColor * 2.0 - 1.0);

    //Tangent Space
    vec3 N = normalize(fragIn.TangentView * nTangentSpace);

    //Color del MapColor
    vec4 texColor = texture(colorMap, fragIn.uv);

    // Luz en View Space
    vec3 lightPosVS = (camera * vec4(light.position, 1.0)).xyz;
    vec3 Ldir = normalize(lightPosVS - fragIn.posVS);
    vec3 Vdir = normalize(-fragIn.posVS);

    //ADS
    FragColor = ADS_withTexture(light, material, N, Ldir, Vdir, texColor);
}