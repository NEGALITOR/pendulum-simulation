#ifndef PHASESPACEPLOT_H
#define PHASESPACEPLOT_H

#include "globals.h"
#include "structs.h"
#include "glObjects.h"



class PhaseSpacePlot
{

public:

    GLVAO    axesVAO;
    GLBuffer axesVBO;
    GLVAO    gridVAO;
    GLBuffer gridVBO;
    GLVAO    trajectoryVAO;
    GLBuffer trajectoryVBO;
    GLVAO    backgroundVAO;
    GLBuffer backgroundVBO;

    void renderPhaseSpacePlot(Shader phaseSpaceShader);

    // Initialize the shader program and buffers
    void initPhaseSpaceShaders();

private:    

    // Update the trajectory buffer with the latest phase space points
    void updateTrajectoryBuffer();

    
};


#endif