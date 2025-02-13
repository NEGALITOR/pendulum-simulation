#include "model.h"

#include "shader.h"
#include "mesh.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(const char *path)
{
    loadModel(path);
}

Model::Model(const char *path, glm::vec3 color)
{
    setColor(color);
    loadModel(path);
}

void Model::Draw(Shader &shader)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
    {
        //cout << meshes.size() << endl;
        meshes[i].Draw(shader);
    }
}


void Model::loadModel(string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        
        vertices.push_back(vertex);
    }


    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];

        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]); 
            //cout << face.mIndices[j] << endl;
        }
                   
    }

    //cout << color.x << endl;
    
    if (color.x == -1.0f)
    {
        return Mesh(vertices, indices, textures, glm::vec3{1.0f, 0.0f, 0.0f});
    }
    else
    {
        return Mesh(vertices, indices, textures, color);
        
    }
}



void Model::setPosition(glm::vec3 pos)
{
    this->position = pos;
}

void Model::setRotation(glm::vec3 rot)
{
    this->rotation = rot;
}

void Model::setScale(glm::vec3 scale)
{
    this->scale = scale;
}

void Model::setColor(glm::vec3 color)
{
    this->color = color;
}