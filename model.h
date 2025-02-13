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

class Model 
{
    public:
        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};
        glm::vec3 color{-1.0f};

        void setPosition(glm::vec3 pos);
        void setRotation(glm::vec3 rot);
        void setScale(glm::vec3 scale);
        void setColor(glm::vec3 color);        

        Model(const char *path);
        Model(const char *path, glm::vec3 color);
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