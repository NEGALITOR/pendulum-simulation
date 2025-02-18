#ifndef STRUCTS_H
#define STRUCTS_H

#include "opengl.h"


// ASSIMP INTEGRATION

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
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


#endif