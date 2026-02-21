#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 camera;
uniform mat4 projection;
uniform mat4 model;

uniform float amplitude;
uniform float phase;
uniform float frecuency;

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

//Ejercicio Calcular la normal de la superficie, para esto se necesita calcular la derivada parcial de Y con respecto a X y con respecto a Z y agregar las ondas
float Func(float x, float z, float amplitude, float phase, float frecuency)
{
    return amplitude * sin(frecuency + (x + z) + phase);
}

void main()
{
    //Obtener geometria que queremos
    mat4 modelView = camera * model; // matriz de modelo-vista
    vec4 newPosition = model * vPosition; // posicion del vertice en el espacio del mundo
    vec4 posVS = camera * newPosition; // posicion del vertice en el espacio de vista

    //Calculamos la normal de la superficie con las ondas
    vec3 Vtz = vec3(newPosition.x, Func(newPosition.x, newPosition.z + 0.01, amplitude, phase, frecuency), newPosition.z + 0.01); // posicion del vertice con las ondas en Z
    vec3 Vtx = vec3(newPosition.x + 0.01, Func(newPosition.x + 0.01, newPosition.z, amplitude, phase, frecuency), newPosition.z); // posicion del vertice con las ondas en X

    vec3 normal = cross(Vtz - newPosition.xyz, Vtx - newPosition.xyz); // normal de la superficie sin normalizar

    //Ahora aplicamos transformaciones
    mat4 matForNormal = transpose(inverse(modelView)); // matriz para transformar normales
    vec3 newNormal = normalize((matForNormal * vec4(normal, 0.0)).xyz); // normal transformada y normalizada
    newPosition.y = Func(newPosition.x, newPosition.z, amplitude, phase, frecuency); // agregar las ondas a la posicion del vertice

    //Calcular la iluminacion
    vec3 lightPosVS = (camera * vec4(light.position, 1.0)).xyz; // posicion de la luz en el espacio de vista
    vec3 Ldir = normalize(lightPosVS - posVS.xyz); // direccion de la luz (desde el punto a la luz)
    vec3 Vdir = normalize(-posVS.xyz); // direccion de la vista (desde el punto a la camara)

    //Calculamos el ADS con todo ya transformado
    outColor = ADS(light, material, newNormal, Ldir, Vdir);

    gl_Position = projection * posVS; // posicion final del vertice en el espacio de recorte
}
