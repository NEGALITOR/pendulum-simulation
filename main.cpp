#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float spinZ, deltaSpin;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, projLoc, lookAtLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, lookAtMat;

string readFile(const char *filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() {
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vfprogram = glCreateProgram();

	string vertShaderStr = readFile("vertShader.glsl");
	string fragShaderStr = readFile("fragShader.glsl");
	const char *vertShaderSrc = vertShaderStr.c_str();
	const char *fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vertexShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fragmentShader, 1, &fragShaderSrc, NULL);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// The following code checks to see if the vertex and fragment
	// fragment shaders compiled properly on the GPU.

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	glAttachShader(vfprogram, vertexShader);
	glAttachShader(vfprogram, fragmentShader);
	glLinkProgram(vfprogram);

	return vfprogram;
}


void setupVertices(void) {
	// Set up 12 triangles with 2 per each cube face.  That's 36 vertices
	// or 108 points.
	float vertexPositions[108] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};

	float vertexColors[108] = {
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f 
	};	


	// Use ONE vao as before	
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	// Use TWO virtual buffer objects, vbo[0] is for the vertexPositions
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);

	// vbo[1] is for the vertexColors 
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColors), vertexColors, GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();
	// These are the camera positions and cube locations.  We will need them to
	// set up the modelview and perspective matrices.  Note the scope of the variables.
	cameraX  = -8.0f; cameraY  = -5.0f;  cameraZ  = 3.0f;
	cubeLocX = 0.0f;  cubeLocY =  0.0f;  cubeLocZ = 0.0f;
	spinZ = 0.0f;
	deltaSpin=0.2;
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {

	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);  // clear the background to black, each time

	glUseProgram(renderingProgram);

	// These are getting the memory locations of the modelview and projection
	// matrices from the renderer
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	lookAtLoc = glGetUniformLocation(renderingProgram, "lookAt_matrix");

	// Now we set the perspective based on the window size and the aspect ratio.
	// The glm::perspective function rerturn the projection matrix
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	// Now we are going to move the camera and cube to set up the modelview matrix.
	//  -- NOTE -- now using lookAt to specify camera position, so set the vmat to
	//  the identity matrix using glm:translate
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	// Now let's also leave the cube at the center of the scene
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));

	// Using the modelview matrix, start the cube rotating about the Z-axis
	mMat = glm::rotate(mMat, glm::radians(spinZ), glm::vec3(0.0f, 0.0f, 1.0f));
	// Since vMat, mMat, and mvMat are all defined as glm::mat4, OpenGL undestands that 
	// the following line is actually MATRIX MULTIPLICATION and produces the modelvies 
	// matrix from vMat and mMat.
	mvMat = vMat * mMat;

	// Now let's set up the lookAt matrix
	lookAtMat = glm::lookAt( glm::vec3(cameraX,cameraY,cameraZ), 
			glm::vec3(cubeLocX, cubeLocY, cubeLocZ),
			glm::vec3(0.0f, 0.0f, 1.0f) );

	// Now place the modelview and projection matrices in the memory locations
	// we recovered earlier from the renderer.
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(lookAtLoc, 1, GL_FALSE, glm::value_ptr(lookAtMat));

	// Now we are going to bind the vertex attribute array buffer and then 
	// tell GL what type of data is in the vbo.  They are
	// floating point vertices arranged as 4 component vectors.
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	// And now we tell GL that this is the array we want to draw with glDrawArrays
	glEnableVertexAttribArray(0);
	// And now the colors
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
	// And now we tell GL that this is the array we want to draw with glDrawArrays
	glEnableVertexAttribArray(1);

	// Turn on the Z-Buffer Depth Test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Now draw the 36 vertices in the vao 
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Under Construction", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		spinZ = spinZ + deltaSpin;
		if ( spinZ > 360.0 ) { spinZ = 0.0; }
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
