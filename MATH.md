# Pendulum Simulation — Math

## Equation of Motion

The pendulum in question is modeled after a damped nonlinear pendulum. The exact second order differential equation (DE) is as follows:

$$\ddot{\theta} = -b\dot{\theta} - \frac{g}{L}\sin\theta$$

where:

| Symbol | Description |
|--------|-------------|
| $\theta$ | Angle from vertical (radians) |
| $\dot{\theta}$ | Angular velocity |
| $\ddot{\theta}$ | Angular acceleration |
| $b$ | Damping coefficient |
| $g$ | Gravitational acceleration ($9.80665\ \text{m/s}^2$) |
| $L$ | Pendulum length |

The $\sin\theta$ term is kept exact instead of being approximated as $\theta$.

---

## State Vector

The second order DE needs to be split into two first order DEs by treating $(\theta,\ \dot{\theta})$ as:

$$\frac{d}{dt}\begin{pmatrix}\theta \\ \dot{\theta}\end{pmatrix} = \begin{pmatrix}\dot{\theta} \\ -b\dot{\theta} - \dfrac{g}{L}\sin\theta\end{pmatrix}$$

---

## 4th Order Runge-Kutta (RK4)

Every frame the state of the pendulum is moved by time elapsing using $h = \Delta t$ using the the 4th order Runge-Kutta (RK4) method:

$$k_1 = f(s)$$
$$k_2 = f\!\left(s + \tfrac{h}{2}k_1\right)$$
$$k_3 = f\!\left(s + \tfrac{h}{2}k_2\right)$$
$$k_4 = f\!\left(s + h\,k_3\right)$$

$$s_{n+1} = s_n + \frac{h}{6}\left(k_1 + 2k_2 + 2k_3 + k_4\right)$$

where $f$ is the derivative function above and $s = (\theta,\ \dot{\theta})$.

RK4 has a good local truncation error of $O(h^5)$ and a global error of $O(h^4)$, making it accurate enough for real-time simulation without any fixed sub-steps. Feel free to read up on proof regarding local and global truncation error of RK4, however it is out of the scope of this program. All that needs to be noted is that it is good for our program for accuracy and speed.

## Phase Space Plot

The simulation plots the trajectory in a **phase space plot**: the 2D plane with $\theta$ on the horizontal axis and $\dot{\theta}$ on the vertical axis.

- An undamped pendulum traces a closed orbit (ellipse for small angles)
- A damped pendulum spirals inward toward the fixed point $(0, 0)$
- The plot axes span $\theta \in [-\pi, \pi]$ and $\dot{\theta} \in [-10, 10]$

---

## Period Detection

The sim measures the oscillation period by detecting the crossing of $\dot{\theta}$ for a chnage of direction via sign change. The elapsed time is recorded as the period $T$.

For small angles, the linear approx. gives $$T \approx 2\pi\sqrt{\frac{L}{g}}$$

but the simulation avoids doing this as we can just calculate it in real time during the simulation instead of wasting resources for a calculation.

---

## Frame Timing and Physics Coupling

The physics step size $h$ is not fixed. It equals the actual elapsed time between rendered frames:

```cpp
double deltaTime = currentTime - lastTime;
updatePendulum(deltaTime);
```

This means the simulation is **frame-rate dependent by design**. A frame that takes 16 ms needs ot be syncronized so the simulation for accurate results.

The following are consequences of the frame rate varying:

| Scenario | Effect |
|----------|--------|
| High FPS (e.g. 120 fps, $h \approx 8\ \text{ms}$) | More steps, lower per step error, slightly more CPU work per second |
| Low FPS (e.g. 30 fps, $h \approx 33\ \text{ms}$) | Fewer steps, higher per step error, but RK4 keeps it acceptable |
| Frame spike (e.g. $h = 200\ \text{ms}$) | Single large step; RK4 may lose accuracy but won't explode |

To make the simulation fully frame-rate independent with guaranteed accuracy, a **fixed sub-step** approach can be used: accumulate elapsed time and consume it in fixed chunks:

```cpp
// Delta time between frames
u_steps = deltaTime;

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

```
This decouples render rate from physics rate entirely and relies on RK4 to keep drift negligible at every frame.

---

## Default Parameters

These are the default parameters I used as default, however they can be changed via CLI arguments (refer to **README** on usage).

| Parameter | Symbol | Value |
|-----------|--------|-------|
| Initial angle | $\theta_0$ | $\pi/2$ rad |
| Damping | $b$ | $0.0025$ |
| Gravity | $g$ | $9.80665\ \text{m/s}^2$ |
| Length | $L$ | $1.0\ \text{m}$ |
