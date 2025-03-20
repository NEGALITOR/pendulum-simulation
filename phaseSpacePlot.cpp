#include "structs.h"
#include "globals.h"

std::vector<std::pair<float, float>> phaseTrajectory;

// Add this function to render the phase space plot
void renderPhaseSpacePlot(GLFWwindow* window)
{
    
    // Get the current window size
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    
    // Define the plot area in the lower right corner (1/4 of the window)
    int plotWidth = windowWidth / 4;
    int plotHeight = windowHeight / 4;
    int plotX = windowWidth - plotWidth - 10;  // 10 pixel margin
    int plotY = 10;  // 10 pixel margin from bottom
    
    // Save current viewport and projection settings
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    // Set new viewport for the phase plot
    glViewport(plotX, plotY, plotWidth, plotHeight);
    
    // Set up orthographic projection for 2D rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    // Define the plot range: [-π to π] for theta and [-10 to 10] for theta_dot
    const float thetaRange = 2.0f * M_PI;
    const float thetaDotRange = 20.0f;
    glOrtho(-thetaRange/2, thetaRange/2, -thetaDotRange/2, thetaDotRange/2, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Draw plot background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.1f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(-thetaRange/2, -thetaDotRange/2);
    glVertex2f(thetaRange/2, -thetaDotRange/2);
    glVertex2f(thetaRange/2, thetaDotRange/2);
    glVertex2f(-thetaRange/2, thetaDotRange/2);
    glEnd();
    
    // Draw plot axes
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    // X-axis (theta)
    glVertex2f(-thetaRange/2, 0.0f);
    glVertex2f(thetaRange/2, 0.0f);
    // Y-axis (theta_dot)
    glVertex2f(0.0f, -thetaDotRange/2);
    glVertex2f(0.0f, thetaDotRange/2);
    glEnd();
    
    // Draw grid lines
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    // Horizontal grid lines
    for (float y = -thetaDotRange/2; y <= thetaDotRange/2; y += 2.0f) {
        glVertex2f(-thetaRange/2, y);
        glVertex2f(thetaRange/2, y);
    }
    // Vertical grid lines
    for (float x = -thetaRange/2; x <= thetaRange/2; x += M_PI/2) {
        glVertex2f(x, -thetaDotRange/2);
        glVertex2f(x, thetaDotRange/2);
    }
    glEnd();
    
    // Draw trajectory points
    if (phaseTrajectory.size() > 1) {
        glColor3f(0.0f, 1.0f, 0.0f);  // Green trajectory
        glBegin(GL_LINE_STRIP);
        for (const auto& point : phaseTrajectory) {
            glVertex2f(point.first, point.second);
        }
        glEnd();
        
        // Draw current point as a larger dot
        glPointSize(5.0f);
        glColor3f(1.0f, 0.0f, 0.0f);  // Red current point
        glBegin(GL_POINTS);
        glVertex2f(phaseTrajectory.back().first, phaseTrajectory.back().second);
        glEnd();
        glPointSize(1.0f);
    }
    
    // Draw labels for axes
    // Note: In a real implementation, you'd use a text rendering library like FreeType or FTGL
    // This is a placeholder for where text would be rendered
    
    // Restore original settings
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    // Restore original viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glDisable(GL_BLEND);
}