#version 460 core

layout (location = 0) in vec4 vPosition;

uniform mat4 camera;
uniform mat4 projection;
uniform mat4 model;

uniform float amplitude;
uniform float phase;
uniform float frecuency;   // se conserva el nombre del código viejo

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
    // Patrón tipo “gota” (radial): depende de x^2 + z^2
    return amplitude * cos(phase + frecuency * (x * x + z * z));
}

// Derivadas parciales analíticas para obtener las tangentes (y con eso la normal)
float DyDx(float x, float z, float amplitude, float phase, float frecuency)
{
    // y = A cos(phase + f(x^2+z^2))
    // dy/dx = -A sin(arg) * (2 f x)
    float arg = phase + frecuency * (x * x + z * z);
    return -amplitude * sin(arg) * (2.0 * frecuency * x);
}

float DyDz(float x, float z, float amplitude, float phase, float frecuency)
{
    float arg = phase + frecuency * (x * x + z * z);
    return -amplitude * sin(arg) * (2.0 * frecuency * z);
}

void main()
{
    //Obtener geometria que queremos
    mat4 modelView = camera * model; // matriz de modelo-vista

    // Ahora primero hacemos la ondulación en espacio objeto y luego transformamos correctamente.
    vec4 newPosition = vPosition; // posicion del vertice en el espacio objeto
    newPosition.y = Func(newPosition.x, newPosition.z, amplitude, phase, frecuency); // agregar las ondas a la posicion del vertice

    vec4 posVS = modelView * newPosition; // posicion del vertice en el espacio de vista

    //Calculamos la normal de la superficie con las ondas
    vec3 Vtx = vec3(1.0, DyDx(vPosition.x, vPosition.z, amplitude, phase, frecuency), 0.0); // vector tangente en X
    vec3 Vtz = vec3(0.0, DyDz(vPosition.x, vPosition.z, amplitude, phase, frecuency), 1.0); // vector tangente en Z

    vec3 normal = normalize(cross(Vtz, Vtx)); // normal de la superficie

    //Ahora aplicamos transformaciones
    mat4 matForNormal = transpose(inverse(modelView)); // matriz para transformar normales
    vec3 newNormal = normalize((matForNormal * vec4(normal, 0.0)).xyz); // normal transformada y normalizada

    //Calcular la iluminacion
    vec3 lightPosVS = (camera * vec4(light.position, 1.0)).xyz;
    vec3 Ldir = normalize(lightPosVS - posVS.xyz);
    vec3 Vdir = normalize(-posVS.xyz);

    //Calculamos el ADS con todo ya transformado
    outColor = ADS(light, material, newNormal, Ldir, Vdir);

    gl_Position = projection * posVS;
}