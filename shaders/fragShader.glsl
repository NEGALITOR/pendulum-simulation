#version 430

//in vec4 color;
in vec2 TexCoords;

out vec4 ex_color;

uniform sampler2D texture_diffuse1;

void main(void)
{	
	ex_color = texture(texture_diffuse1, TexCoords);
}
