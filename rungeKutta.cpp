#include "rungeKutta.h"

#include "structs.h"
#include "globals.h"

// Phsyics
float initialTheta = M_PI/2;
float u_time = 0.0f;         // Accumulated time
float u_steps = 0.01f;       // Time step for RK4
float u_b = 0.0025f; //0.0025f;            // Damping coefficient (b in equation)
float u_g = 9.80665f;           // Gravity (g in equation)
float u_L = 1.0f;            // Length of pendulum (L in equation)
float u_theta = initialTheta;      // Current angle (θ)
float u_theta_dot = 0.0f;    // Current angular velocity (θ̇)

// Period
static float lastTimeCrossed = 0.0f;
static bool firstPass = true;
static bool secondPass = false;


PendulumState derivative(const PendulumState& state) 
{
    PendulumState dState;
    dState.theta = state.theta_dot;
    dState.theta_dot = -(u_b * state.theta_dot + (u_g / u_L) * sin(state.theta));
    return dState;
}

// Helper function for RK4 calculations
PendulumState addState(const PendulumState& state, const PendulumState& deriv, float scale) {
    PendulumState result;
    result.theta = state.theta + scale * deriv.theta;
    result.theta_dot = state.theta_dot + scale * deriv.theta_dot;
    return result;
}

// 4th-order Runge-Kutta method for pendulum simulation
void updatePendulum(float deltaTime) {
    // If simulation is stopped, don't update state
    if (isPendulumStopped) return;
    
    // Accumulate time
    u_time += deltaTime;
    
    // Current state
    PendulumState state = {u_theta, u_theta_dot};
    
    // Calculate k1
    PendulumState k1 = derivative(state);
    
    // Calculate k2
    PendulumState state2 = addState(state, k1, u_steps / 2.0f);
    PendulumState k2 = derivative(state2);
    
    // Calculate k3
    PendulumState state3 = addState(state, k2, u_steps / 2.0f);
    PendulumState k3 = derivative(state3);
    
    // Calculate k4
    PendulumState state4 = addState(state, k3, u_steps);
    PendulumState k4 = derivative(state4);
    
    // Update angle and angular velocity
    u_theta += (u_steps / 6.0f) * (k1.theta + 2.0f * k2.theta + 2.0f * k3.theta + k4.theta);
    u_theta_dot += (u_steps / 6.0f) * (k1.theta_dot + 2.0f * k2.theta_dot + 2.0f * k3.theta_dot + k4.theta_dot);

    phaseTrajectory.push_back({u_theta, u_theta_dot});
    // 
    // Detect period by checking when θ crosses initial angle (pi/2)
    if (!secondPass && u_theta_dot > 0) 
    {
        if (!firstPass) {
            float period = u_time - lastTimeCrossed;
            printf("Measured Period: %.3f seconds\n", period);
            secondPass = true;
        }
        lastTimeCrossed = u_time;
        firstPass = false;
    }

}