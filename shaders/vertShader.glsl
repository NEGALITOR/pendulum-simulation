#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 textureCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 lookAt_matrix;

void main(void)
{
	TexCoords = textureCoords;
	FragPos = vec3(mv_matrix * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(mv_matrix))) * normal;
	gl_Position = proj_matrix * lookAt_matrix * mv_matrix * vec4(position, 1.0);
} 
