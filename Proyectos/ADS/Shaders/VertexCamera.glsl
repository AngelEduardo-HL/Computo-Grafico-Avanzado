#version 460 core

layout (location = 0) in vec4 vPosition;
//layout (location = 1) in vec4 vColor;

uniform float time;
uniform mat4 camera;
uniform mat4 projection;

void main ()
{  
    vec4 newPosition = vPosition;
	newPosition.y = newPosition.y + cos(time);
	//newPosition.y = 0.5 * cos(vPosition.x^2 + vPosition.z^2);
	newPosition = projection * camera * newPosition;
	
	gl_Position = newPosition;  //equivale a hacer return gl_Position	
}


//Y = 0.5 * cos(x^2 + z^2);
//magnitud, frecuencia. Función donde se vayan a crear los valores y eso ocupa la formula