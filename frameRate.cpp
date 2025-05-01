// Add to your implementation file
double frameTime = 0.0;
double frameRate = 0.0;
bool showProperties = true;

// Function to calculate frame rate
void calculateFrameRate(double currentTime) {
    static double lastTime = 0.0;
    static int frameCount = 0;
    static double frameRateUpdateTime = 0.0;
    
    // Update frame count
    frameCount++;
    
    // Calculate time difference
    double timeDiff = currentTime - frameRateUpdateTime;
    
    // Update frame rate every 0.5 seconds
    if (timeDiff >= 0.5) {
        frameRate = frameCount / timeDiff;
        frameCount = 0;
        frameRateUpdateTime = currentTime;
    }
    
    // Calculate frame time (time taken to render one frame)
    frameTime = currentTime - lastTime;
    lastTime = currentTime;
}