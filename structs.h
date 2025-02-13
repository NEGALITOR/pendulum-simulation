#ifndef STRUCTS_H
#define STRUCTS_H

#include "opengl.h"

#include <stdint.h>
#include <array>

/*struct Model
{
    GLuint vao[1], vbo[2];

    uint32_t triangleCount;
    GLfloat* vertices;
    uint32_t vertexCount;
    GLfloat* normals;
    uint32_t normalCount;

    glm::vec3 position = {0,0,0};
    glm::vec3 rotation = {0,0,0};
    glm::vec3 scale = {1,1,1};
    glm::vec3 color;
    
};*/



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