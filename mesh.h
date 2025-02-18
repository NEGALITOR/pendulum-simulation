#ifndef MESH_H
#define MESH_H

#include "structs.h"
#include "shader.h"

#include <vector>

class Mesh
{
    public:
        GLuint VAO[1], VBO[2], EBO[0];
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, glm::vec3 color);
        void Draw(Shader &shader);

    private:
        void setupMesh(glm::vec3 color);
        vector<glm::vec3> fillColors(int numVerts, glm::vec3 color);
};

#endif