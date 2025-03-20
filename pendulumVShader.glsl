#version 430

// this will receive the positions that were defined in the vbo and
// set up into 3 component vectors in the vao. The "in" in the declaration
// stands for "input" to the verted shader.  The layout (location=0)
// can be used to select a particular buffer.

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 textureCoords;
//layout (location=3) in vec3 in_color;

//out vec2 color;
out vec2 TexCoords;

const float PI = 3.14159265359;

//uniform float u_time;         // Accumulated time
uniform float u_steps;       // Time step for RK4
uniform float u_b;            // Damping coefficient (b in equation)
uniform float u_g;           // Gravity (g in equation)
uniform float u_L;            // Length of pendulum (L in equation)
uniform float u_theta;      // Current angle (θ)
uniform float u_theta_dot;    // Current angular velocity (θ̇)
uniform float u_delta_time;


// It is more efficient to do the matrix work of setting up the display in the shader
// on the GPU than in the application program.  OpenGL lets us set these variables
// up as "uniform" variables in the shader.  The following three variable hold
// the contents of the 4x4 modelview and projection matrices and the lookat matrix
// that were defined in the display function of the application.  

uniform mat4 m_matrix;
uniform mat4 v_matrix;
uniform mat4 proj_matrix;
uniform mat4 lookAt_matrix;



struct PendulumState {
    float theta;      // Angle
    float theta_dot;  // Angular velocity
};

PendulumState derivative(const PendulumState state) {
    PendulumState dState;
    dState.theta = state.theta_dot;
    dState.theta_dot = -(u_b * state.theta_dot + (u_g / u_L) * sin(state.theta));
    return dState;
}

// Helper function for RK4 calculations
PendulumState addState(const PendulumState state, const PendulumState deriv, float scale) {
    PendulumState result;
    result.theta = state.theta + scale * deriv.theta;
    result.theta_dot = state.theta_dot + scale * deriv.theta_dot;
    return result;
}

// 4th-order Runge-Kutta method for pendulum simulation
PendulumState updatePendulum(float deltaTime) {
    float theta = u_theta;
	float theta_dot = u_theta_dot;
    
    
    // Accumulate time
    
    
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
    theta += (u_steps / 6.0f) * (k1.theta + 2.0f * k2.theta + 2.0f * k3.theta + k4.theta);
    theta_dot += (u_steps / 6.0f) * (k1.theta_dot + 2.0f * k2.theta_dot + 2.0f * k3.theta_dot + k4.theta_dot);

	PendulumState final = {theta, theta_dot};
	return final;
}

void main(void)
{
	//float lastTime = 0.0;
    //float deltaTime = currentTime - lastTime;
    //lastTime = currentTime;

	PendulumState state = updatePendulum(u_delta_time);

	mat4 rotMat = mat4(
		cos(state.theta), 0.0, sin(state.theta), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(state.theta), 0.0, cos(state.theta), 0.0,
		0.0, 0.0, 0.0, 1.0
	);

    rotMat = m_matrix * rotMat;
	mat4 mv_matrix = v_matrix * rotMat;

	// here we transform our point to the correct position and projection using
	// matrix multiplication using operations tied to the vec4 data type 

        // Remember, matrix multiplication happens right to left...

	TexCoords = textureCoords;
	gl_Position = proj_matrix * lookAt_matrix * mv_matrix * vec4(position,1.0f);
	//color = vec4(in_color,1.0f);
} 
