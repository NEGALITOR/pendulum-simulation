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
    
        
        vector<Mesh> meshes;
        vector<Texture> textures_loaded;
        string directory;

        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};

        void setPosition(glm::vec3 pos);
        void setRotation(glm::vec3 rot);
        void setScale(glm::vec3 scale);

        Model(const char *path);
        void Draw(Shader &shader);
        
       
    private:

        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(const aiScene *scene, aiMaterial *mat, aiTextureType type, string typeName);
        unsigned int TextureFromFile(const aiScene *scene, aiString path);
};


#endif