#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <cmath>
#include <memory>

#include "opengl.h"

#include "model.h"
#include "shader.h"
#include "textRender.h"

// Custom deleter so unique_ptr calls glfwDestroyWindow on scope exit
struct GLFWwindowDeleter {
    void operator()(GLFWwindow* w) const { glfwDestroyWindow(w); }
};
using UniqueWindow = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>;

extern UniqueWindow window;
const GLuint WINDOW_WIDTH = 600;
extern int width, height;

extern unsigned int frames;
extern float pendulumPeriod;

extern bool isPendulumStopped;

extern TextRender textRenderer;;

extern std::vector<std::pair<float, float>> phaseTrajectory;

// Phsyics
extern float initialTheta;
extern float u_time;        // Accumulated time
extern float u_steps;       // Time step for RK4
extern float u_b;           // Damping coefficient (b in equation)
extern float u_g;           // Gravity (g in equation)
extern float u_L;           // Length of pendulum (L in equation)
extern float u_theta;       // Current angle (θ)
extern float u_theta_dot;   // Current angular velocity (θ̇)

extern float user_b;
extern float user_L;
extern float user_g;


extern vector<Shader> shaders;


#endif