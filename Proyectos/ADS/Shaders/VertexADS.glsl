#version 460 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColor;

out vec4 vertexColor;
uniform float time;
uniform mat4 cam;
uniform mat4 projection;
uniform mat4 object;


struct Light 
{
	vec3 position;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};


uniform Light light;
uniform Material material;
uniform vec3 eye;

vec3 Normal = vec3(0.0, 1.0, 0.0);



vec4 Ambient(struct Light l, struct Material m)
{
	return light.ambient * material.ambient;
}

vec4 Diffuse(struct Light l, struct Material m, vec3 Normal)
{
	vec4 resultMultDiffuse = light.diffuse * material.diffuse;
	float resultDot = dot(light.position, Normal);
	return resultMultDiffuse * resultDot;
}

vec4 Specular(struct Light l, struct Material m, vec3 eye, vec3 Normal)
{
	vec4 resultMultSpecular = -light.specular * material.specular;
	vec3 refl = reflect(vec3(0.0,0.0,0.0), Normal);
	float resultDot = dot(eye, refl);
	return resultMultSpecular * resultDot;
}

vec4 ADS(Light l, Material m, vec3 Normal, vec3 eye )
{
	return Ambient(l, m) + Diffuse(l, m, Normal) + Specular(l, m, eye, Normal);
}



out vec4 outColor;




void main ()
{  		
	//gl_Position = projection * cam * object * vPosition;
	mat4 accum = object; //Le defino la acumulacion de matrices a mi objeto

	vec4 Space = cam * accum * vPosition;



	//transpuesta de la matriz
	mat4 matForNormal = transpose(inverse(cam * accum));


	//Normal del plano
	vec3 newNormal = normalize((matForNormal * vec4(Normal, 0.0)).xyz);


	vec3 SpaceLight = (cam * vec4(light.position, 1.0)).xyz; //La posicion de la luz en el espacio
	vec3 DirectionLight = normalize(SpaceLight - Space.xyz); //La dirección de la luz
	vec3 DirectionView = normalize(-Space.xyz); //Hacia donde va a pegar la luz


	//ADS
	outColor = ADS(light, material, newNormal, eye);


	gl_Position = projection * Space;


	//Operaciones de transformaciones
	//outColor = ADS();
	//vec4 newPosition = vPosition;
	//Aplicar al vertice las transformaciones
	//Primero new pos con todas las transformaciones = myPosition, después calculo de ADS
	//gl_Position = newPosition;  
}