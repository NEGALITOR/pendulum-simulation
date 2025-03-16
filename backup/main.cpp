#include "opengl.h"
#include "prototypes.h"
#include "structs.h"

#include "model.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <cmath>

using namespace std;

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float spinZ, deltaSpin;
float radius = 10.0f; // Distance from camera to center
float horizontalAngle = 0.0f; // Angle for left/right movement
float verticalAngle = 0.0f; // Angle for up/down movement
float originalSpinRate = 0.2f; // Store original rotation rate
bool isPendulumStopped = false;

// variable allocation for display
GLuint mvLoc, projLoc, lookAtLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, lookAtMat;

vector<Model> models;
vector<Shader> shaders;
uint32_t globalVertexCount;

// Mouse callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_RIGHT:
				if (isPendulumStopped) 
				{
					isPendulumStopped = false;
					models[2].setColor(glm::vec3(0.0f, 0.0f, 0.75f));
				}
                deltaSpin += 0.5f; // Increase rotation speed
                break;
            case GLFW_MOUSE_BUTTON_LEFT:
                deltaSpin -= 0.5f; // Decrease rotation speed
                if (deltaSpin <= 0.0f) 
				{
					isPendulumStopped = true;
                    deltaSpin = 0.0f;
                    // Change pendulum color to red when stopped
                    models[2].setColor(glm::vec3(1.0f, 0.0f, 0.0f));
                }
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                // Reset to original position and speed
                spinZ = 0.0f;
                deltaSpin = originalSpinRate;
                isPendulumStopped = false;
                models[2].setColor(glm::vec3(0.0f, 0.0f, 0.75f)); // Reset to original color
                break;
        }
    }
}

// Keyboard callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const float moveSpeed = 0.02f;
    const float radiusSpeed = 0.05f;
    //if (action == GLFW_REPEAT) cout << "done" <<endl;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) 
	{
        switch (key) 
		{
            case GLFW_KEY_LEFT:
                horizontalAngle -= moveSpeed;
                break;
            case GLFW_KEY_RIGHT:
                horizontalAngle += moveSpeed;
                break;
            case GLFW_KEY_UP:
                verticalAngle += moveSpeed;
                verticalAngle = glm::min(verticalAngle, 1.5f); // Limit vertical angle
                break;
            case GLFW_KEY_DOWN:
                verticalAngle -= moveSpeed;
                verticalAngle = glm::max(verticalAngle, -1.5f); // Limit vertical angle
                break;
            case GLFW_KEY_PAGE_UP:
                radius -= radiusSpeed;
                radius = glm::max(radius, 2.0f); // Minimum radius
                break;
            case GLFW_KEY_PAGE_DOWN:
                radius += radiusSpeed;
                radius = glm::min(radius, 20.0f); // Maximum radius
                break;
        }
        
        // Update camera position based on spherical coordinates
        cameraX = radius * cos(verticalAngle) * cos(horizontalAngle);
        cameraY = radius * cos(verticalAngle) * sin(horizontalAngle);
        cameraZ = radius * sin(verticalAngle);
    }
}

void setupVertices(void) {

	Shader testS("vertShader.glsl", "fragShader.glsl");
	shaders.push_back(testS);

	//Model test("models/glb/base.glb");
	/*Model base("models/glb/base.glb", glm::vec3{0.75f, 0.0f, 0.75f});
	base.setPosition(glm::vec3 {0.0f, 0.0f, -1.0f});
	models.push_back(base);
	
	Model support("models/glb/support.glb", glm::vec3{0.0f, 0.75f, 0.0f});
	support.setPosition(glm::vec3 {2.0f, 0.0f, -0.5f});
	models.push_back(support);*/

	Model pendulum("../models/glb/pendulum.glb");
	pendulum.setPosition(glm::vec3 {0.0f, 0.0f, 3.0f});
	pendulum.setRotation(glm::vec3{0.0f, 0.0f, 90.0f});
	pendulum.setScale(glm::vec3{0.6f, 0.6f, 0.6f});
	models.push_back(pendulum);	

}

void init(GLFWwindow* window) {
	//renderingProgram = createShaderProgram();
	// These are the camera positions  align verticesand cube locations.  We will need them to
	// set up the modelview and perspective matrices.  Note the scope of the variables.
    cameraX = -8.0f; cameraY = -5.0f; cameraZ = 3.0f;
    cubeLocX = 0.0f; cubeLocY = 0.0f; cubeLocZ = 0.0f;

	radius = sqrt(cameraX * cameraX + cameraY * cameraY + cameraZ * cameraZ);
    horizontalAngle = atan2(cameraY, cameraX);
    verticalAngle = asin(cameraZ / radius);

    spinZ = 0.0f;
    deltaSpin = originalSpinRate;
    
    // Set up callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    setupVertices();
}

void display(GLFWwindow* window, double currentTime) {

	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }

	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/*for (int i = 0; i < 2; i++)
	{
		
		//glUseProgram(renderingProgram);
		shaders[0].use();

		// Get uniform locations
		mvLoc = glGetUniformLocation(shaders[0].ID, "mv_matrix");
		projLoc = glGetUniformLocation(shaders[0].ID, "proj_matrix");
		lookAtLoc = glGetUniformLocation(shaders[0].ID, "lookAt_matrix");

		glfwGetFramebufferSize(window, &width, &height);
		aspect = (float)width / (float)height;
		pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
		vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		lookAtMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(cubeLocX, cubeLocY, cubeLocZ), glm::vec3(0.0f, 0.0f, 1.0f));

		mMat = glm::mat4(1.0f);
		mMat = glm::translate(mMat, models[i].position);
		mMat = glm::translate(mMat, glm::vec3(0.0f, 0.0f, 0.0f));
		mMat = glm::scale(mMat, models[i].scale);
		mMat = glm::rotate(mMat, glm::radians(models[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		mMat = glm::rotate(mMat, glm::radians(models[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		mMat = glm::rotate(mMat, glm::radians(models[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		

		mvMat = vMat * mMat;

		shaders[0].setMat4("mv_matrix", mvMat);
		shaders[0].setMat4("proj_matrix", pMat);
		shaders[0].setMat4("lookAt_matrix", lookAtMat);

		models[i].Draw(shaders[0]);
	}
	*/

	shaders[0].use();

	// Get uniform locations
	mvLoc = glGetUniformLocation(shaders[0].ID, "mv_matrix");
	projLoc = glGetUniformLocation(shaders[0].ID, "proj_matrix");
	lookAtLoc = glGetUniformLocation(shaders[0].ID, "lookAt_matrix");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	lookAtMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(cubeLocX, cubeLocY, cubeLocZ), glm::vec3(0.0f, 0.0f, 1.0f));

	mMat = glm::mat4(1.0f);

	
	
	mMat = glm::translate(mMat, models[0].position);
	mMat = glm::scale(mMat, models[0].scale);

	mMat = glm::translate(mMat, glm::vec3(0.0f, 0.0f, 0.5f));
	
	mMat = glm::rotate(mMat, glm::radians(models[0].rotation.x+spinZ), glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, glm::radians(models[0].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, glm::radians(models[0].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	mMat = glm::translate(mMat, glm::vec3(0.0f, 0.0f, -0.5f));
	

	mvMat = vMat * mMat;

	shaders[0].setMat4("mv_matrix", mvMat);
	shaders[0].setMat4("proj_matrix", pMat);
	shaders[0].setMat4("lookAt_matrix", lookAtMat);

	models[2].Draw(shaders[0]);

}


int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(600, 600, "Pendulum Simulation", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    glfwSwapInterval(1);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
		spinZ = spinZ + deltaSpin;
		if (spinZ > 360.0) { spinZ = 0.0; }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
