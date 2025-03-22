#ifndef PHASESPACEPLOT_H
#define PHASESPACEPLOT_H

#include "globals.h"
#include "structs.h"



class PhaseSpacePlot
{

public:

    GLuint axesVAO, axesVBO;
    GLuint gridVAO, gridVBO;
    GLuint trajectoryVAO, trajectoryVBO;
    void renderPhaseSpacePlot(Shader phaseSpaceShader);

    // Initialize the shader program and buffers
    void initPhaseSpaceShaders();

private:    
    
    // Clean up the shader resources
    void cleanupPhaseSpaceShaders(Shader phaseSpaceShader);

    // Update the trajectory buffer with the latest phase space points
    void updateTrajectoryBuffer();

    
};


#endif