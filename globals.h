#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <cmath>

#include "opengl.h"

#include "model.h"
#include "shader.h"

extern GLFWwindow* window;
const GLuint WINDOW_WIDTH = 600;
extern int width, height;

extern unsigned int frames;

extern bool isPendulumStopped;

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


extern vector<Shader> shaders;


#endif