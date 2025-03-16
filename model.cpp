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

#include "includes/SOIL2/soil2.h"


Model::Model(const char *path)
{
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
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | 
                                                    aiProcess_CalcTangentSpace | aiProcess_FixInfacingNormals | aiProcess_GenUVCoords);

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
        vertex.Position = vector;
        
        
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        
        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

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


    //cout << sizeof(scene->mMaterials)/sizeof(scene->mMaterials[0]) << endl;

    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    

        // diffuse
        vector<Texture> diffuseMaps = loadMaterialTextures(scene, material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        
        // specular maps
        vector<Texture> specularMaps = loadMaterialTextures(scene, material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(scene, material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(scene, material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
    
    return Mesh(vertices, indices, textures);

}

vector<Texture> Model::loadMaterialTextures(const aiScene *scene, aiMaterial *mat, aiTextureType type, string typeName)
{
    
    vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) 
    {
        aiString texturePath;
        mat->GetTexture(type, i, &texturePath);

        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), texturePath.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }

        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(scene, texturePath);
            texture.type = typeName;
            texture.path = texturePath.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }

    
    return textures;
    
}

unsigned int Model::TextureFromFile(const aiScene *scene, aiString path)
{

    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    // Check if the texture is embedded (indicated by '*' at the start)
    if (path.C_Str()[0] == '*') 
    {
        int texIndex = std::stoi(&path.C_Str()[1]);
        aiTexture* texture = scene->mTextures[texIndex];
        int width, height, channels;

        if (texture) 
        {
            // Extract embedded texture data
            size_t dataSize = texture->mWidth;  // mWidth holds the size of the data
            std::vector<unsigned char> textureData(texture->mWidth);
            std::memcpy(textureData.data(), texture->pcData, dataSize);

            // Load the image from memory using SOIL2
            
            //SOIL_load_image
            unsigned char* imgData = SOIL_load_image_from_memory(
                textureData.data(),   // Pointer to the raw texture data
                textureData.size(),   // Size of the data
                &width, &height, &channels, SOIL_LOAD_AUTO
            );

            if (imgData) {
                std::cout << "Image loaded successfully: " << width << "x" << height << "x" << channels << std::endl;
                
                GLenum format;
                if (channels == 1)
                    format = GL_RED;
                else if (channels == 3)
                    format = GL_RGB;
                else if (channels == 4)
                    format = GL_RGBA;
        
                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);
                glGenerateMipmap(GL_TEXTURE_2D);
        
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // You can now use imgData in OpenGL (e.g., to create a texture)
                // Don't forget to free the image data when done!
                SOIL_free_image_data(imgData);
            } else {
                std::cerr << "Failed to load image from memory!" << std::endl;
                SOIL_free_image_data(imgData);
            }
        }
        
    }
    return textureID;
        
    
}

/*
vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    
    vector<Texture> textures;

    unsigned int textureCount = mat->GetTextureCount(type);
    //cout << "Found " << textureCount << " textures of type " << typeName << endl;

    
    

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {

        aiString str;
        aiReturn ret = mat->Get(AI_MATKEY_TEXTURE(type, i), str);

        //cout << str.C_Str() << endl;

        //aiReturn result = mat->GetTexture(type, i, &str);        
        
        //aiString str;
        //mat->GetTexture(type, i, &str);

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
    
}

unsigned int Model::TextureFromFile(const char *path, const string &directory)
{
    
    string filename = string(path);
    filename = directory + '/' + filename;
    //cout << "Filepath: " << filename << endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, 0);

    //cout << "Loaded texture data: " << (data ? "success" : "failure") << endl;

    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        SOIL_free_image_data(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        //SOIL_free_image_data(data);
    }

    return textureID;
}
*/

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


