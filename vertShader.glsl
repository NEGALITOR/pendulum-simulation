#version 430

// this will receive the positions that were defined in the vbo and
// set up into 3 component vectors in the vao. The "in" in the declaration
// stands for "input" to the verted shader.  The layout (location=0)
// can be used to select a particular buffer.

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 textureCoords;
layout (location=3) in vec3 in_color;

out vec4 color;

// It is more efficient to do the matrix work of setting up the display in the shader
// on the GPU than in the application program.  OpenGL lets us set these variables
// up as "uniform" variables in the shader.  The following three variable hold
// the contents of the 4x4 modelview and projection matrices and the lookat matrix
// that were defined in the display function of the application.  

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 lookAt_matrix;

void main(void)
{
	// here we transform our point to the correct position and projection using
	// matrix multiplication using operations tied to the vec4 data type 

        // Remember, matrix multiplication happens right to left...

	gl_Position = proj_matrix * lookAt_matrix * mv_matrix * vec4(position,1.0f);
	color = vec4(in_color,1.0f);
} 
