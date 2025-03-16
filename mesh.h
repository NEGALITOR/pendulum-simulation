#ifndef MESH_H
#define MESH_H

#include "structs.h"
#include "shader.h"

#include <vector>

class Mesh
{
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        unsigned int VAO, VBO, EBO;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void Draw(Shader &shader);

    private:
        void setupMesh();
};

#endif