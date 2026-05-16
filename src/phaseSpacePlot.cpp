#include "structs.h"
#include "globals.h"
#include "phaseSpacePlot.h"

std::vector<std::pair<float, float>> phaseTrajectory;

#include <string>
#include <iostream>

GLuint axesVAO, axesVBO;
GLuint gridVAO, gridVBO;
GLuint trajectoryVAO, trajectoryVBO;

// Initialize the shader program and buffers
void PhaseSpacePlot::initPhaseSpaceShaders() 
{
    Shader phaseSpaceShader("shaders/phaseSpaceVShader.glsl", "shaders/phaseSpaceFShader.glsl");
    
    // Create VAO and VBO for axes spanning the full theta and angular velocity range
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
    // The buffer is updated every frame with new theta/theta_dot pairs
    glGenVertexArrays(1, &trajectoryVAO);
    glGenBuffers(1, &trajectoryVBO);
    
    glBindVertexArray(trajectoryVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trajectoryVBO);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    shaders.push_back(phaseSpaceShader);
}

// Clean up the shader resources
void PhaseSpacePlot::cleanupPhaseSpaceShaders(Shader phaseSpaceShader) 
{
    glDeleteProgram(phaseSpaceShader.getID());
    glDeleteVertexArrays(1, &axesVAO);
    glDeleteBuffers(1, &axesVBO);
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &trajectoryVAO);
    glDeleteBuffers(1, &trajectoryVBO);
}


// Update the trajectory buffer with the latest phase space points
void PhaseSpacePlot::updateTrajectoryBuffer() 
{
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
void PhaseSpacePlot::renderPhaseSpacePlot(Shader phaseSpaceShader) 
{
    
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
    phaseSpaceShader.use();
    
    // Create projection matrix (orthographic)
    const float thetaRange = 2.0f * M_PI;
    const float thetaDotRange = 20.0f;
    glm::mat4 projection = glm::ortho(-thetaRange/2, thetaRange/2, -thetaDotRange/2, thetaDotRange/2, -1.0f, 1.0f);
    
    // Set the projection matrix in the shader
    phaseSpaceShader.setMat4("projection", projection);
    phaseSpaceShader.setMat4("model", glm::mat4(1.0f));
    
    // Draw plot background
    phaseSpaceShader.setVec4("color", glm::vec4(0.1f, 0.1f, 0.1f, 0.7f));
    
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
    phaseSpaceShader.setVec4("color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, 20); // Adjust the count based on your grid lines
    
    // Draw axes
    phaseSpaceShader.setVec4("color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, 4); // 2 lines (X and Y axes) with 2 points each
    
    // Update and draw trajectory
    if (phaseTrajectory.size() > 1) {
        updateTrajectoryBuffer();
        
        // Draw trajectory line
        phaseSpaceShader.setVec4("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        glBindVertexArray(trajectoryVAO);
        glDrawArrays(GL_LINE_STRIP, 0, phaseTrajectory.size());
        
        // Highlight the current state position as a single oversized dot
        glPointSize(5.0f);
        phaseSpaceShader.setVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
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