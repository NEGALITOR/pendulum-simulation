#version 430

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

// Define a light struct
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

// Support up to 8 lights
#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform vec3 viewPos;

// Global lighting parameters
uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.5;
uniform float shininess = 32.0;

void main(void)
{
    // Ambient base lighting
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    
    // Combined lighting from all sources
    vec3 lighting = ambient;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Process each light
    for(int i = 0; i < numLights && i < MAX_LIGHTS; i++) {
        // Diffuse
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * lights[i].intensity;
        
        // Specular
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularStrength * spec * lights[i].color * lights[i].intensity;
        
        // Add this light's contribution
        lighting += diffuse + specular;
    }
    
    // Apply lighting to texture
    vec4 texColor = texture(texture_diffuse1, TexCoords);
    if(texColor.a < 0.1)
        discard;
    vec3 result = lighting * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}
