#ifndef LOADMODEL
#define LOADMODEL

#include "opengl.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <random>

vector<float> loadMesh(const char* filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        //return 0;
    }

    // Assuming the first mesh is the one we want (modify this for more meshes)
    aiMesh* mesh = scene->mMeshes[0];

    // Prepare vertex data
    vector<float> vertices;

    //cout << mesh->mNumVertices << endl;
    
    // Extract the vertex positions from the mesh
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D position = mesh->mVertices[i];
        vertices.push_back(position.x);
        vertices.push_back(position.z);
        vertices.push_back(position.y);
    }

    //for(int i=0; i < vertices.size(); i+=3)
    //    printf("%.2f %.2f %.2f\n", vertices[i], vertices[i+1], vertices[i+2]);

    return vertices;
    
   
}

/*
vector<float> randomColors(int numVerts)
{
    vector<float> colors;
    float color;

    for (unsigned int i = 0; i < numVerts; i+=3)
    {
        colors.push_back(float(rand()) / RAND_MAX);
        colors.push_back(float(rand()) / RAND_MAX);
        colors.push_back(float(rand()) / RAND_MAX);
    }
    return colors;
}
*/

#endif
