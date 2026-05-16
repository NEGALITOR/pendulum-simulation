
#include "light.h"
#include "globals.h"

Light::Light(glm::vec3 position, glm::vec3 color, float intensity, 
             float ambientStrength, float specularStrength, float shininess)
    : position(position), color(color), intensity(intensity),
      ambientStrength(ambientStrength), specularStrength(specularStrength), shininess(shininess)
{
}

void LightManager::addLight(const Light& light)
{
    // Silently discard lights that exceed the maximum supported count
    if (lights.size() < MAX_LIGHTS) {
        lights.push_back(light);
    }
}

void LightManager::applyLights(Shader& shader, const glm::vec3& viewPos)
{
    shader.setVec3("viewPos", viewPos);
    shader.setInt("numLights", static_cast<int>(lights.size()));
    
    // Use the first light's parameters as shared ambient and specular settings
    if (!lights.empty()) {
        shader.setFloat("ambientStrength", lights[0].getAmbientStrength());
        shader.setFloat("specularStrength", lights[0].getSpecularStrength());
        shader.setFloat("shininess", lights[0].getShininess());
    }
    
    // Upload each light's position, color, and intensity into the shader array uniform
    for (size_t i = 0; i < lights.size(); i++) {
        std::string prefix = "lights[" + std::to_string(i) + "].";
        shader.setVec3(prefix + "position", lights[i].getPosition());
        shader.setVec3(prefix + "color", lights[i].getColor());
        shader.setFloat(prefix + "intensity", lights[i].getIntensity());
    }
}

