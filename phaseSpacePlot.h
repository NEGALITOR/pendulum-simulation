#ifndef PHASESPACEPLOT_H
#define PHASESPACEPLOT_H

#include "globals.h"
#include "structs.h"

extern GLuint phaseSpaceShader;
extern GLuint axesVAO, axesVBO;
extern GLuint gridVAO, gridVBO;
extern GLuint trajectoryVAO, trajectoryVBO;
extern bool shadersInitialized;

class PhaseSpacePlot
{

public:
    void renderPhaseSpacePlot(GLFWwindow* window);

private:
    GLuint createShaderProgram();

    // Initialize the shader program and buffers
    void initPhaseSpaceShaders();
    // Clean up the shader resources
    void cleanupPhaseSpaceShaders();

    // Helper function to set a uniform matrix in the shader
    void setMat4(GLuint shader, const char* name, const glm::mat4& mat);

    // Helper function to set a uniform vector in the shader
    void setVec4(GLuint shader, const char* name, const glm::vec4& value);

    // Update the trajectory buffer with the latest phase space points
    void updateTrajectoryBuffer();

    
};


#endif