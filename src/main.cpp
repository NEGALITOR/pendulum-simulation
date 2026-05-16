#include <string>
#include <fstream>


#include "globals.h"
#include "structs.h"

#include "phaseSpacePlot.h"
#include "rungeKutta.h"

#include "light.h"

GLFWwindow* window;
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
unsigned int frames = 0;

bool isPendulumStopped = false;

vector<Model> models;
vector<Shader> shaders;
PhaseSpacePlot plot;
TextRender textRenderer;

// Add to globals section
LightManager lightManager;

// Mouse callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
            case GLFW_MOUSE_BUTTON_RIGHT:
                //deltaSpin += 0.5f; // Increase rotation speed
                isPendulumStopped = true;
                //models[3].setColor(glm::vec3(0.0f, 0.0f, 0.75f));
                break;
            case GLFW_MOUSE_BUTTON_LEFT:
                //deltaSpin -= 0.5f; // Decrease rotation speed

                isPendulumStopped = false;
                //deltaSpin = 0.0f;
                // Change pendulum color to red when stopped
                //models[3].setColor(glm::vec3(1.0f, 0.0f, 0.0f));

                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                // Reset to original position and speed
                //spinZ = 0.0f;
                //deltaSpin = originalSpinRate;
                isPendulumStopped = false;

                u_time = 0.0f;         // Accumulated time
                u_steps = 0.00f;       // Base time step for RK4
                u_b = user_b;            // Damping coefficient (b in equation)
                u_g = user_g;           // Gravity (g in equation)
                u_L = user_L;            // Length of pendulum (L in equation)
                u_theta = initialTheta;      // Current angle (θ)
                u_theta_dot = 0.0f;    // Current angular velocity (θ̇)

                //models[3].setColor(glm::vec3(0.0f, 0.0f, 0.75f)); // Reset to original color
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

	Shader staticShader("shaders/vertShader.glsl", "shaders/fragShader.glsl");
	shaders.push_back(staticShader);

    Shader pendulumShader("shaders/vertShader.glsl", "shaders/fragShader.glsl");
	shaders.push_back(pendulumShader);

    Model room("models/glb/room/room.glb");
	room.setPosition(glm::vec3 {0.0f, -2.0f, -12.0f});
    room.setRotation(glm::vec3{0.0f, 0.0f, -35.0f});
	models.push_back(room);

    Model table("models/glb/table/table.glb");
	table.setPosition(glm::vec3 {0.0f, -2.0f, -12.0f});
    table.setRotation(glm::vec3{0.0f, 0.0f, -35.0f});
	models.push_back(table);
    
    Model chair("models/glb/chair/chair.glb");
	chair.setPosition(glm::vec3 {0.0f, -2.0f, -12.0f});
    chair.setRotation(glm::vec3{0.0f, 0.0f, -35.0f});
	models.push_back(chair);
    
    Model trash("models/glb/trash/trash.glb");
	trash.setPosition(glm::vec3 {0.0f, -2.0f, -12.0f});
    trash.setRotation(glm::vec3{0.0f, 0.0f, -35.0f});
	models.push_back(trash);

    Model whiteboard("models/glb/whiteboard/whiteboard.glb");
	whiteboard.setPosition(glm::vec3 {0.0f, -2.0f, -12.0f});
    whiteboard.setRotation(glm::vec3{0.0f, 0.0f, -35.0f});
	models.push_back(whiteboard);

    Model lamp("models/glb/lamp/lamp.glb");
    lamp.setPosition(glm::vec3 {7.0f, -3.0f, -1.5f});
    models.push_back(lamp);

    Model rooflamp("models/glb/rooflamp/rooflamp.glb");
    rooflamp.setPosition(glm::vec3 {0.0f, 0.0f, 25.0f});
    models.push_back(rooflamp);

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

    
    plot.initPhaseSpaceShaders();

    textRenderer.initTextRenderShaders(50);

    // Set up lights with custom parameters
    Light lampLight(
        glm::vec3(7.0f, -3.0f, 3.0f),     // position
        glm::vec3(1.0f, 0.9f, 0.8f),      // diffuse color 
        0.6f,                             // intensity
        0.1f,                             // ambient strength
        0.1f,                             // specular strength
        0.2f                             // shininess
    );

    Light roofLampLight(
        glm::vec3(0.0f, 0.0f, 10.0f),     // position
        glm::vec3(0.8f, 0.8f, 1.0f),      // diffuse color
        0.5f,                             // intensity
        0.01f,                             // ambient strength
        0.5f,                             // specular strength
        0.2f                             // shininess 
    );

    Light ambientLight(
        glm::vec3(0.0f, 0.0f, 0.0f),      // position (doesn't matter for ambient)
        glm::vec3(0.2f, 0.2f, 0.3f),      // diffuse color (slight blue tint)
        0.5f,                             // intensity
        0.2f,                             // ambient strength
        0.0f,                             // specular strength
        0.2f                              // shininess
    );

    lightManager.addLight(lampLight);
    //lightManager.addLight(roofLampLight);
    lightManager.addLight(ambientLight);
}

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
    
    // Set up callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    setupVertices();
    

}


void display(GLFWwindow* window, double currentTime) {

    static double lastTime = 0.0;
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    
    updatePendulum(deltaTime);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "OpenGL error: " << err << std::endl;
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    int i;
    // Draw base and support first
    for (i = 0; i < models.size()-1; i++) {
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

        lightManager.applyLights(shaders[0], glm::vec3(cameraX, cameraY, cameraZ));
        
        models[i].Draw(shaders[0]);
    }
    //printf("%d\n", i);

    // Draw the pendulum
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
    mMat = glm::translate(mMat, models[i].position);
    mMat = glm::scale(mMat, models[i].scale);

    // Apply rotation based on physics
    mMat = glm::rotate(mMat, u_theta, glm::vec3(1.0f, 0.0f, 0.0f));

    mMat = glm::rotate(mMat, glm::radians(models[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    mvMat = vMat * mMat;

    shaders[1].setMat4("mv_matrix", mvMat);
    shaders[1].setMat4("proj_matrix", pMat);
    shaders[1].setMat4("lookAt_matrix", lookAtMat);

    lightManager.applyLights(shaders[1], glm::vec3(cameraX, cameraY, cameraZ));

    models[i].Draw(shaders[1]);

    plot.renderPhaseSpacePlot(shaders[2]);

    // Render FPS text
    static string fpsText = "FPS: ";
    static double frameLastTime = 0.0;
    double frameDeltaTime = currentTime - frameLastTime;
    frames++;
    
    if (frameDeltaTime >= 1.0f)
    {
        fpsText = "FPS: " + to_string(frames);
        
        frames = 0;
        frameLastTime = currentTime;
    }
    textRenderer.renderText(shaders[3], fpsText, 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    
    // Display the period on screen
    string periodText = "Period: ";
    if (pendulumPeriod > 0.0f)
    {
        periodText = "Period: " + to_string(pendulumPeriod).substr(0, 5) + " s";
    }
    textRenderer.renderText(shaders[3], periodText, 25.0f, 75.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

    string angleText = "Angle: " + to_string(glm::degrees(u_theta)).substr(0, 6);
    textRenderer.renderText(shaders[3], angleText, 25.0f, 125.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    string dampText = "Damp: " + to_string(u_b).substr(0, 4);
    textRenderer.renderText(shaders[3], dampText, 25.0f, 175.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    string lengthText = "Length: " + to_string(u_L).substr(0, 4);
    textRenderer.renderText(shaders[3], lengthText, 25.0f, 225.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    string gravityText = "Gravity: " + to_string(u_g).substr(0, 4); 
    textRenderer.renderText(shaders[3], gravityText, 25.0f, 275.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
}

int main(int argc, char* argv[]) 
{
    if (!glfwInit()) { exit(EXIT_FAILURE); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_WIDTH, "Pendulum Simulation", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    //glfwSwapInterval(1);
    
    if (argc > 1)
    {
        //cout << argv[1] << endl;
        float angle = atof(argv[1]);
        if (angle <= 180 && angle >= 0)
            initialTheta = glm::radians(angle);
    }
    if (argc > 2)
    {
        user_b = atof(argv[2]);
        u_b = user_b;
    }
    if (argc > 3)
    {
        user_L = atof(argv[3]);
        u_L = user_L;
    }
    if (argc > 4)
    {
        user_g = atof(argv[4]);
        u_g = user_g;
    }

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
