# Pendulum Simulation

Real-time 3D damped pendulum simulation rendered with OpenGL, including a live phase-space plot and on-screen telemetry.

## Video Explanation
Note: The code in the video is an implementation I made a year ago, however the logic should follow regardless.  
https://drive.google.com/file/d/1cFP4FPt79auF1QMU16CCRoFMzxNcDLFH/view?usp=sharing

## What This Application Does

- Simulates pendulum dynamics using a 4th-order Runge-Kutta integrator
- Renders a 3D room scene with a physically driven pendulum model
- Displays real-time stats (FPS, period, angle, damping, length, gravity)
- Shows the phase trajectory in a dedicated viewport

## Physics Model

**NOTE! REFER TO MATH.md FOR MORE COMPREHENSIVE MATH EXPLANATION!**

The simulation integrates:

- theta dot = angular velocity
- theta double dot = -b * theta dot + (g / L) * sin(theta)

Where:

- b is damping coefficient
- g is gravity
- L is pendulum length
- theta is angular displacement

Integration is done per frame using RK4 with frame delta time

## Dependencies

This project uses:

- CMake
- OpenGL
- GLEW
- GLFW
- Assimp
- FreeType
- C/C++ compiler toolchain
- SOIL2 (SOIL2 is fetched and built from source automatically by CMake)

This project is only supported on X11, however it can be adjusted to wayland by downloading the ```glfw-wayland``` instead.

## Tested Platforms

- **Arch Linux** — KDE Plasma on X11
- **Fedora** — KDE Plasma on X11
- **macOS Sequoia** — tested via Homebrew dependencies

### OpenGL Version Notes

- macOS builds with OpenGL 4.1 and GLSL 410
- Linux and Windows build with OpenGL 4.3 and GLSL 430

### Fedora KDE (dnf)

```bash
sudo dnf install -y \
  cmake make gcc-c++ git \
  mesa-libGL-devel \
  glew-devel glfw-devel assimp-devel freetype-devel
```

For the KDE Plasma X11 session:

```bash
sudo dnf install -y plasma-workspace-x11
```

### Arch Linux (pacman)

```bash
sudo pacman -S --needed \
  cmake base-devel git \
  mesa glew glfw-x11 assimp freetype2
```

For the KDE Plasma X11 session:

```bash
sudo pacman -S --needed plasma-x11-session
```

### macOS (Homebrew)

```bash
brew install cmake glew glfw assimp freetype pkg-config
```

## Build and Run

From the project root:

```bash
cmake -S . -B build
cmake --build build
./build/pendulum
```

## Command-Line Parameters

You can override initial simulation values when launching:

```bash
./build/pendulum [angle_deg] [damping_b] [length_L] [gravity_g]
```

Examples:

```bash
./build/pendulum
./build/pendulum 45
./build/pendulum 45 0.01 1.5 9.81
```

Parameter notes:

- angle_deg: initial angle in degrees, valid range 0 to 180
- damping_b: damping coefficient
- length_L: pendulum length
- gravity_g: gravity constant

## Controls

Mouse:

- Left click: resume simulation
- Right click: pause simulation
- Middle click: reset simulation state and user-tuned parameters

Keyboard:

- Arrow keys: orbit camera
- Page Up and Page Down: zoom in and out
- R: reset angle and angular velocity
- B and V: increase or decrease damping
- G and F: increase or decrease gravity
- L and K: increase or decrease pendulum length

## Runtime UI

On-screen text includes:

- FPS
- estimated pendulum period
- current angle
- damping value
- pendulum length
- gravity value

The phase-space panel appears in the lower-right of the window

## Project Layout

- src: C++ source files
- include: header files
- shaders: GLSL shaders
- models: 3D assets
- fonts: text rendering assets

## Troubleshooting

- If CMake cannot find a dependency, verify the package is installed for your distro and rerun configure
- On macOS, OpenGL is limited by the platform and runs in 4.1 mode by design
- If runtime assets are missing, rebuild so post-build asset copy runs again




