#ifndef STRUCTS_H
#define STRUCTS_H

#include "opengl.h"



// ASSIMP INTEGRATION

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
};

struct Texture
{
    unsigned int id;
    string type;
    string path;
};

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};


struct PendulumState 
{
    float theta;      // Angle
    float theta_dot;  // Angular velocity
};


#endif