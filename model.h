#ifndef MODEL_H
#define MODEL_H

#include "shader.h"
#include "mesh.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/*
class Model
{
    public:
        vector<Texture> loadTextures;
        vector<Mesh> meshes;
        string dir;
        bool gammaCorrection;

        Model(string const &path, bool gamma = false);

        void Draw(Shader &shader);

    private:
        void loadModel(string const &path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
        unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);
};
*/

class Model 
{
    public:
        glm::vec3 color = {-1.0f, -1.0f, -1.0f};

        Model(const char *path);
        void Draw(Shader &shader);
       
    private:
        // model data
        vector<Mesh> meshes;
        string directory;

        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
        unsigned int TextureFromFile(const char *path, const string &directory);
};


#endif