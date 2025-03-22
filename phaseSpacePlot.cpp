#include "structs.h"
#include "globals.h"
#include "phaseSpacePlot.h"

std::vector<std::pair<float, float>> phaseTrajectory;

// Add these includes at the top of your file
#include <string>
#include <iostream>

// Shader compilation and program linking
GLuint PhaseSpacePlot::createShaderProgram() {

    string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;

	try
	{
		vShaderFile.open("shaders/phaseSpaceVShader.glsl");
		fShaderFile.open("shaders/phaseSpaceFShader.glsl");
		stringstream vShaderStream, fShaderstream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderstream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderstream.str();
	}
	catch (ifstream::failure &e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_PROPERLY_READ: " << e.what() << endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();

    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    
    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    
    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

// Global variables for rendering
GLuint phaseSpaceShader = 0;
GLuint axesVAO = 0, axesVBO = 0;
GLuint gridVAO = 0, gridVBO = 0;
GLuint trajectoryVAO = 0, trajectoryVBO = 0;
bool shadersInitialized = false;

// Initialize the shader program and buffers
void PhaseSpacePlot::initPhaseSpaceShaders() {
    // Create shader program
    phaseSpaceShader = createShaderProgram();
    
    // Create VAO and VBO for axes
    float axesVertices[] = {
        // X-axis
        -M_PI, 0.0f,
        M_PI, 0.0f,
        // Y-axis
        0.0f, -10.0f,
        0.0f, 10.0f
    };
    
    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);
    
    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axesVertices), axesVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Create VAO and VBO for grid
    std::vector<float> gridVertices;
    
    // Horizontal grid lines
    for (float y = -10.0f; y <= 10.0f; y += 2.0f) {
        gridVertices.push_back(-M_PI);
        gridVertices.push_back(y);
        gridVertices.push_back(M_PI);
        gridVertices.push_back(y);
    }
    
    // Vertical grid lines
    for (float x = -M_PI; x <= M_PI; x += M_PI/2) {
        gridVertices.push_back(x);
        gridVertices.push_back(-10.0f);
        gridVertices.push_back(x);
        gridVertices.push_back(10.0f);
    }
    
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Create VAO and VBO for trajectory
    glGenVertexArrays(1, &trajectoryVAO);
    glGenBuffers(1, &trajectoryVBO);
    
    glBindVertexArray(trajectoryVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trajectoryVBO);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    shadersInitialized = true;
}

// Clean up the shader resources
void PhaseSpacePlot::cleanupPhaseSpaceShaders() {
    if (shadersInitialized) {
        glDeleteProgram(phaseSpaceShader);
        glDeleteVertexArrays(1, &axesVAO);
        glDeleteBuffers(1, &axesVBO);
        glDeleteVertexArrays(1, &gridVAO);
        glDeleteBuffers(1, &gridVBO);
        glDeleteVertexArrays(1, &trajectoryVAO);
        glDeleteBuffers(1, &trajectoryVBO);
        shadersInitialized = false;
    }
}

// Helper function to set a uniform matrix in the shader
void PhaseSpacePlot::setMat4(GLuint shader, const char* name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, glm::value_ptr(mat));
}

// Helper function to set a uniform vector in the shader
void PhaseSpacePlot::setVec4(GLuint shader, const char* name, const glm::vec4& value) {
    glUniform4fv(glGetUniformLocation(shader, name), 1, glm::value_ptr(value));
}

// Update the trajectory buffer with the latest phase space points
void PhaseSpacePlot::updateTrajectoryBuffer() {
    if (!phaseTrajectory.empty()) {
        std::vector<float> trajectoryVertices;
        trajectoryVertices.reserve(phaseTrajectory.size() * 2);
        
        for (const auto& point : phaseTrajectory) {
            trajectoryVertices.push_back(point.first);
            trajectoryVertices.push_back(point.second);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, trajectoryVBO);
        glBufferData(GL_ARRAY_BUFFER, trajectoryVertices.size() * sizeof(float), trajectoryVertices.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

// Render the phase space plot using shaders
void PhaseSpacePlot::renderPhaseSpacePlot(GLFWwindow* window) {
    // Initialize shaders if not already done
    if (!shadersInitialized) {
        initPhaseSpaceShaders();
    }
    
    // Get the current window size
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    
    // Define the plot area in the lower right corner (1/4 of the window)
    int plotWidth = windowWidth / 4;
    int plotHeight = windowHeight / 4;
    int plotX = windowWidth - plotWidth - 10;  // 10 pixel margin
    int plotY = 10;  // 10 pixel margin from bottom
    
    // Save current viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // Set new viewport for the phase plot
    glViewport(plotX, plotY, plotWidth, plotHeight);
    
    // Enable blending for transparent background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Use our shader program
    glUseProgram(phaseSpaceShader);
    
    // Create projection matrix (orthographic)
    const float thetaRange = 2.0f * M_PI;
    const float thetaDotRange = 20.0f;
    glm::mat4 projection = glm::ortho(-thetaRange/2, thetaRange/2, -thetaDotRange/2, thetaDotRange/2, -1.0f, 1.0f);
    
    // Set the projection matrix in the shader
    setMat4(phaseSpaceShader, "projection", projection);
    setMat4(phaseSpaceShader, "model", glm::mat4(1.0f)); // Identity model matrix
    
    // Draw plot background
    setVec4(phaseSpaceShader, "color", glm::vec4(0.1f, 0.1f, 0.1f, 0.7f));
    
    float backgroundVertices[] = {
        -thetaRange/2, -thetaDotRange/2,
        thetaRange/2, -thetaDotRange/2,
        thetaRange/2, thetaDotRange/2,
        -thetaRange/2, thetaDotRange/2
    };
    
    GLuint backgroundVAO, backgroundVBO;
    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    
    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDeleteVertexArrays(1, &backgroundVAO);
    glDeleteBuffers(1, &backgroundVBO);
    
    // Draw grid
    setVec4(phaseSpaceShader, "color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, 20); // Adjust the count based on your grid lines
    
    // Draw axes
    setVec4(phaseSpaceShader, "color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, 4); // 2 lines (X and Y axes) with 2 points each
    
    // Update and draw trajectory
    if (phaseTrajectory.size() > 1) {
        updateTrajectoryBuffer();
        
        // Draw trajectory line
        setVec4(phaseSpaceShader, "color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        glBindVertexArray(trajectoryVAO);
        glDrawArrays(GL_LINE_STRIP, 0, phaseTrajectory.size());
        
        // Draw current point as a larger dot
        glPointSize(5.0f);
        setVec4(phaseSpaceShader, "color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        glDrawArrays(GL_POINTS, phaseTrajectory.size() - 1, 1);
        glPointSize(1.0f);
    }
    
    // Cleanup
    glBindVertexArray(0);
    glUseProgram(0);
    
    // Restore original viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glDisable(GL_BLEND);
}