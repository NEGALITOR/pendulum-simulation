#ifndef RUNGEKUTTA_H
#define RUNGEKUTTA_H

#include "structs.h"
#include "globals.h"

PendulumState derivative(const PendulumState& state);

// Helper function for RK4 calculations
PendulumState addState(const PendulumState& state, const PendulumState& deriv, float scale);

// 4th-order Runge-Kutta method for pendulum simulation
void updatePendulum(float deltaTime);

#endif