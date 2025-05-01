#ifndef LIGHT_H
#define LIGHT_H

#include "opengl.h"
#include "shader.h"
#include <vector>

class Light
{
public:
    Light(glm::vec3 position, glm::vec3 color, float intensity = 1.0f, 
          float ambientStrength = 0.1f, float specularStrength = 0.5f, float shininess = 32.0f);
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return color; }
    float getIntensity() const { return intensity; }
    float getAmbientStrength() const { return ambientStrength; }
    float getSpecularStrength() const { return specularStrength; }
    float getShininess() const { return shininess; }
    
    void setPosition(glm::vec3 newPosition) { position = newPosition; }
    void setColor(glm::vec3 newColor) { color = newColor; }
    void setIntensity(float newIntensity) { intensity = newIntensity; }
    void setAmbientStrength(float newAmbientStrength) { ambientStrength = newAmbientStrength; }
    void setSpecularStrength(float newSpecularStrength) { specularStrength = newSpecularStrength; }
    void setShininess(float newShininess) { shininess = newShininess; }

private:
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float ambientStrength;
    float specularStrength;
    float shininess;
};

// Light manager to handle multiple lights
class LightManager
{
public:
    static const int MAX_LIGHTS = 8;
    
    void addLight(const Light& light);
    void applyLights(Shader& shader, const glm::vec3& viewPos);
    
private:
    std::vector<Light> lights;
};

#endif
