#version 460 core

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D texture0;

void main ()
{
    vec4 color = texture(texture0, texCoord);

    if (color.a < 0.01) //Decarta el fragmento si no hay color o es transparente
        discard;

    outColor = color;
}