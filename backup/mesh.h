/*#ifndef MESH_H
#define MESH_H

#include "structs.h"
#include "shader.h"

#include <vector>

class Mesh
{
    public:
        GLuint VAO[1], VBO[2], EBO[1];
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        //Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, glm::vec3 color);
        void Draw(Shader &shader);
        void setupColor(glm::vec3 color);
        

    private:
        //void setupMesh(glm::vec3 color);
        void setupMesh();
        vector<glm::vec3> fillColors(int numVerts, glm::vec3 color);
};

#endif
*/
#ifndef MESH_H
#define MESH_H

#include "structs.h"
#include "shader.h"

#include <vector>

class Mesh
{
    public:
        GLuint VAO[1], VBO[2], EBO[1]; // Changed from EBO[0] to EBO[1]
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        //Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, glm::vec3 color);
        void Draw(Shader &shader);
        void setupColor(glm::vec3 color);
        
        // Add proper copy constructor
        Mesh(const Mesh& other) {
            this->vertices = other.vertices;
            this->indices = other.indices;
            this->textures = other.textures;
            
            // Re-setup the mesh with the copied data
            setupMesh();
        }

    private:
        //void setupMesh(glm::vec3 color);
        void setupMesh();
        vector<glm::vec3> fillColors(int numVerts, glm::vec3 color);
};

#endif