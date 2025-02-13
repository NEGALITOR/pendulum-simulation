#ifndef PROTOTYPES
#define PROTOTYPES

#include "opengl.h"
#include "structs.h"

#include <string>
#include <vector>

//vector<float> loadMesh(const char* filePath);
//vector<float> randomColors(int numVerts);
//void renderMesh(GLuint VAO, int numTris);


//void readSTLFile(const char* filename,  GLfloat **vertices, uint32_t &vertexCount,  GLfloat **normals, uint32_t &normalCount, uint32_t &triangleCount);
Model loadModel(const char* filename, glm::vec3 color);
GLfloat* randomColors(int numVerts);
GLfloat* fillColors(int numVerts, glm::vec3 color);


#endif