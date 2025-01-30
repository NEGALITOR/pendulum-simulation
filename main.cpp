#include "opengl.h"
#include "prototypes.h"
#include "structs.h"

#include <string>
#include <fstream>
#include <cmath>



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

Model models[3];
uint32_t globalVertexCount;

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

	

	const char* modelPaths[] = {
		"models/stl/base.stl",
		"models/stl/support.stl",
		"models/stl/pendulum.stl"
	};

	Model model;
	glm::vec3 color;

	color = {1.0, 0.0, 0.0};
	model = loadModel(modelPaths[0], color);
	models[0] = model;

	color = {0.0, 1.0, 0.0};
	model = loadModel(modelPaths[1], color);
	model.position = {2, 0, 0};
	models[1] = model;

	color = {0.0, 0.0, 1.0};
	model = loadModel(modelPaths[2], color);
	model.position = {0, 0, 3.75};
	model.rotation = {0, 0, 90};
	model.scale = {0.6, 0.6, 0.6};
	models[2] = model;
	
	
	

}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();
	// These are the camera positions  align verticesand cube locations.  We will need them to
	// set up the modelview and perspective matrices.  Note the scope of the variables.
	cameraX  = -8.0f; cameraY  = -5.0f;  cameraZ  = 3.0f;
	cubeLocX = 0.0f;  cubeLocY =  0.0f;  cubeLocZ = 0.0f;
	spinZ = 0.0f;
	deltaSpin = 0.0f;
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {

	glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);

    // Get uniform locations
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    lookAtLoc = glGetUniformLocation(renderingProgram, "lookAt_matrix");

    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    lookAtMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ),
                            glm::vec3(cubeLocX, cubeLocY, cubeLocZ),
                            glm::vec3(0.0f, 0.0f, 1.0f));

    for (int i = 0; i < (sizeof(models) / sizeof(*models)); i++) {
        // Create transformation matrix for each model
        mMat = glm::translate(glm::mat4(1.0f), models[i].position);
		mMat = glm::rotate(mMat, glm::radians(models[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		mMat = glm::rotate(mMat, glm::radians(models[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		mMat = glm::rotate(mMat, glm::radians(models[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		mMat = glm::scale(mMat, models[i].scale);

        // Apply additional transformations per model (example: rotation)
        mMat = glm::rotate(mMat, glm::radians(spinZ), glm::vec3(0.0f, 0.0f, 1.0f));
		//mMat = glm::rotate(mMat, glm::radians(spinZ), models[i].rotation);

        mvMat = vMat * mMat;

        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
        glUniformMatrix4fv(lookAtLoc, 1, GL_FALSE, glm::value_ptr(lookAtMat));

        glBindBuffer(GL_ARRAY_BUFFER, models[i].vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, models[i].vbo[1]);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(1);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glDrawArrays(GL_TRIANGLES, 0, models[i].vertexCount);
    }
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
