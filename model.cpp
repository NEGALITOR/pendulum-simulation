

#include "opengl.h"
#include "structs.h"
#include <string.h>
#include <fstream>

GLfloat* randomColors(int numVerts)
{
    GLfloat* colors = new GLfloat[numVerts];
    float color;

    for (unsigned int i = 0; i < numVerts; i++)
    {
        colors[i] = float(rand()) / RAND_MAX;
    }
    return colors;
}

GLfloat* fillColors(int numVerts, glm::vec3 color)
{
    GLfloat* colors = new GLfloat[numVerts];

    for (unsigned int i = 0; i < numVerts; i+=3)
    {
        colors[i] = color.x;
        colors[i+1] = color.y;
        colors[i+2] = color.z;
    }
    return colors;
}

//void readSTLFile(const char* filename,  GLfloat **vertices, uint32_t &vertexCount,  GLfloat **normals, uint32_t &normalCount, uint32_t &triangleCount)
Model loadModel(const char* filename, glm::vec3 color)
{
    ifstream file(filename, ios::in | ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(1);
    }


    uint8_t header[80]; 
    file.read(reinterpret_cast<char*>(header), 80); 
    //printf("Header: %d\n", header);

    uint32_t triCount;
    file.read(reinterpret_cast<char*>(&triCount), sizeof(triCount));
    //printf("Triangle Count: %d\n", triCount);

    GLfloat* localVerts = new GLfloat[triCount*9];
    GLfloat* localNormals = new GLfloat[triCount*3];

    //uint32_t vertCount = 0;
    //uint32_t normCount = 0;

    for (uint32_t i = 0; i < triCount; ++i)
    {

        glm::vec3 normal;
        file.read(reinterpret_cast<char*>(&normal), sizeof(normal));
        localNormals[i*3] = normal.x;
        localNormals[i*3+1] = normal.y;
        localNormals[i*3+2] = normal.z;
        //normCount += 3;

        glm::vec3 vert[3];
        file.read(reinterpret_cast<char*>(&vert), sizeof(vert));


        uint16_t attribByteCount;
        file.read(reinterpret_cast<char*>(&attribByteCount), sizeof(attribByteCount));

        for (int j = 0; j < 3; ++j)
        {
            localVerts[i * 9 + j * 3]     = vert[j].x;
            localVerts[i * 9 + j * 3 + 1] = vert[j].y;
            localVerts[i * 9 + j * 3 + 2] = vert[j].z;
            
        }

    }
    file.close();

    Model model = {.triangleCount = triCount, .vertices = localVerts, .vertexCount = triCount*9, .normals = localNormals, .normalCount = triCount*3};

    //GLfloat* colors = randomColors(model.vertexCount);
    model.color = color;
    GLfloat* colors = fillColors(model.vertexCount, model.color);

	// Use ONE vao as before	
	glGenVertexArrays(1, model.vao);
	glBindVertexArray(model.vao[0]);
	// Use TWO virtual buffer objects, vbo[0] is for the vertexPositions
	glGenBuffers(2, model.vbo);
	

	glBindBuffer(GL_ARRAY_BUFFER, model.vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, model.vertexCount * sizeof(GLfloat), model.vertices, GL_STATIC_DRAW);
	
	// vbo[1] is for the vertexColors 
	glBindBuffer(GL_ARRAY_BUFFER, model.vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, model.vertexCount * sizeof(GLfloat), colors, GL_STATIC_DRAW);

    return model;   
    
}




