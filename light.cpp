
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
    if (lights.size() < MAX_LIGHTS) {
        lights.push_back(light);
    }
}

void LightManager::applyLights(Shader& shader, const glm::vec3& viewPos)
{
    shader.setVec3("viewPos", viewPos);
    shader.setInt("numLights", static_cast<int>(lights.size()));
    
    // Set global lighting parameters from the first light
    if (!lights.empty()) {
        shader.setFloat("ambientStrength", lights[0].getAmbientStrength());
        shader.setFloat("specularStrength", lights[0].getSpecularStrength());
        shader.setFloat("shininess", lights[0].getShininess());
    }
    
    for (size_t i = 0; i < lights.size(); i++) {
        std::string prefix = "lights[" + std::to_string(i) + "].";
        shader.setVec3(prefix + "position", lights[i].getPosition());
        shader.setVec3(prefix + "color", lights[i].getColor());
        shader.setFloat(prefix + "intensity", lights[i].getIntensity());
    }
}

