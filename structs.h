#ifndef STRUCTS_H
#define STRUCTS_H

#include "opengl.h"

#include <stdint.h>
#include <array>

struct Model
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
    
};


#endif