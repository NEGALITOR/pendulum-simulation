#include <string>
#include <fstream>


#include "globals.h"
#include "prototypes.h"
#include "structs.h"

#include "model.h"
#include "shader.h"

#include "rungeKutta.h"

using namespace std;

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float spinZ, deltaSpin;
float radius = 10.0f; // Distance from camera to center
float horizontalAngle = 0.0f; // Angle for left/right movement
float verticalAngle = 0.0f; // Angle for up/down movement
float originalSpinRate = 0.2f; // Store original rotation rate


// variable allocation for display
GLuint mvLoc, projLoc, lookAtLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, lookAtMat;

vector<Model> models;
vector<Shader> shaders;

bool isPendulumStopped = false;


// Mouse callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_RIGHT:
                //deltaSpin += 0.5f; // Increase rotation speed
                isPendulumStopped = true;
                //models[2].setColor(glm::vec3(0.0f, 0.0f, 0.75f));
                break;
            case GLFW_MOUSE_BUTTON_LEFT:
                //deltaSpin -= 0.5f; // Decrease rotation speed

                isPendulumStopped = false;
                //deltaSpin = 0.0f;
                // Change pendulum color to red when stopped
                //models[2].setColor(glm::vec3(1.0f, 0.0f, 0.0f));

                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                // Reset to original position and speed
                //spinZ = 0.0f;
                //deltaSpin = originalSpinRate;
                isPendulumStopped = false;


                u_time = 0.0f;         // Accumulated time
                u_steps = 0.01f;       // Time step for RK4
                u_b = 0.1f;            // Damping coefficient (b in equation)
                u_g = 9.81f;           // Gravity (g in equation)
                u_L = 1.0f;            // Length of pendulum (L in equation)
                u_theta = M_PI/2;      // Current angle (θ)
                u_theta_dot = 0.0f;    // Current angular velocity (θ̇)

                //models[2].setColor(glm::vec3(0.0f, 0.0f, 0.75f)); // Reset to original color
                break;
        }
    }
}

// Modify the key_callback function to add these controls
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

            case GLFW_KEY_R:
                // Reset theta and theta_dot
                u_theta = M_PI/2;
                u_theta_dot = 0.0f;
                printf("RESET\n");
                break;
            case GLFW_KEY_B:
                // Increase damping
                u_b += 0.05f;
                printf("Damp: %.2f\n", u_b);
                break;
            case GLFW_KEY_V:
                // Decrease damping
                u_b -= 0.05f;
                u_b = glm::max(u_b, 0.0f);
                printf("Damp: %.2f\n", u_b);
                break;
            case GLFW_KEY_G:
                // Increase gravity
                u_g += 0.5f;
                printf("Gravity: %.2f\n", u_g);
                break;
            case GLFW_KEY_F:
                // Decrease gravity
                u_g -= 0.5f;
                u_g = glm::max(u_g, 0.0f);
                printf("Gravity: %.2f\n", u_g);
                break;
            case GLFW_KEY_L:
                // Increase length
                u_L += 0.1f;
                printf("Length: %.2f\n", u_L);
                break;
            case GLFW_KEY_K:
                // Decrease length
                u_L -= 0.1f;
                u_L = glm::max(u_L, 0.1f);
                printf("Length: %.2f\n", u_L);
                break;
        }
        
        // Update camera position based on spherical coordinates
        cameraX = radius * cos(verticalAngle) * cos(horizontalAngle);
        cameraY = radius * cos(verticalAngle) * sin(horizontalAngle);
        cameraZ = radius * sin(verticalAngle);
    }
}

void setupVertices(void) {

	Shader staticShader("vertShader.glsl", "fragShader.glsl");
	shaders.push_back(staticShader);

    Shader pendulumShader("vertShader.glsl", "fragShader.glsl");
	shaders.push_back(pendulumShader);

	//Model test("models/glb/base.glb");
	Model base("models/glb/base/base.glb");
	base.setPosition(glm::vec3 {0.0f, 0.0f, -1.0f});
	models.push_back(base);
	
	Model support("models/glb/support/support.glb");
	support.setPosition(glm::vec3 {2.0f, 0.0f, -0.5f});
	models.push_back(support);

	Model pendulum("models/glb/pendulum/pendulum.glb");
	pendulum.setPosition(glm::vec3 {0.0f, 0.0f, 3.05f});
	pendulum.setRotation(glm::vec3{0.0f, 0.0f, 90.0f});
	pendulum.setScale(glm::vec3{0.6f, 0.6f, 0.6f});
	models.push_back(pendulum);

}

// Modify the init function to initialize pendulum parameters
void init(GLFWwindow* window) {
    //renderingProgram = createShaderProgram();
    // These are the camera positions and cube locations.
    cameraX = -8.0f; cameraY = -5.0f; cameraZ = 3.0f;
    cubeLocX = 0.0f; cubeLocY = 0.0f; cubeLocZ = 0.0f;

    radius = sqrt(cameraX * cameraX + cameraY * cameraY + cameraZ * cameraZ);
    horizontalAngle = atan2(cameraY, cameraX);
    verticalAngle = asin(cameraZ / radius);

    spinZ = 0.0f;
    deltaSpin = originalSpinRate;
    
    // Initialize pendulum variables
    u_time = 0.0f;
    u_theta = M_PI/2;  // Initial angle (45 degrees)
    u_theta_dot = 0.0f; // Initial angular velocity
    
    // Set up callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    setupVertices();
}

// Modify the display function to use RK4 for the pendulum
void display(GLFWwindow* window, double currentTime) {
    static double lastTime = 0.0;
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Update pendulum state using RK4
    updatePendulum(deltaTime);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw base and support first
    for (int i = 0; i < 2; i++) {
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

    // Now draw the pendulum with physics
    shaders[1].use();

    // Get uniform locations
    mvLoc = glGetUniformLocation(shaders[1].ID, "mv_matrix");
    projLoc = glGetUniformLocation(shaders[1].ID, "proj_matrix");
    lookAtLoc = glGetUniformLocation(shaders[1].ID, "lookAt_matrix");

    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    lookAtMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(cubeLocX, cubeLocY, cubeLocZ), glm::vec3(0.0f, 0.0f, 1.0f));

    
    mMat = glm::mat4(1.0f);
    mMat = glm::translate(mMat, models[2].position);
    mMat = glm::scale(mMat, models[2].scale);

    //mMat = glm::translate(mMat, glm::vec3(0.0f, 0.0f, 0.5f));

    // Apply rotation based on physics
    mMat = glm::rotate(mMat, u_theta, glm::vec3(1.0f, 0.0f, 0.0f));

    mMat = glm::rotate(mMat, glm::radians(models[2].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    //mMat = glm::translate(mMat, glm::vec3(0.0f, 0.0f, -0.5f));

    mvMat = vMat * mMat;

    shaders[1].setMat4("mv_matrix", mvMat);
    shaders[1].setMat4("proj_matrix", pMat);
    shaders[1].setMat4("lookAt_matrix", lookAtMat);

    models[2].Draw(shaders[1]);

    //renderPhaseSpacePlot(window);
}

// Modify main function to remove the old spinZ update
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
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}