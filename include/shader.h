#ifndef SHADER_H
#define SHADER_H

#include "structs.h"

#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <sstream>


class Shader
{
    public:
        unsigned int ID;
        Shader();
        Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        void use();
        GLuint getID();
        void deleteProgram();
        
        // Quick Helper functions for setting uniform variables
        void setBool(std::string_view name, bool value) const;
        void setInt(std::string_view name, int value) const;
        void setFloat(std::string_view name, float value) const;
        void setVec2(std::string_view name, const glm::vec2 &value) const;
        void setVec2(std::string_view name, float x, float y) const;
        void setVec3(std::string_view name, const glm::vec3 &value) const;
        void setVec3(std::string_view name, float x, float y, float z) const;
        void setVec4(std::string_view name, const glm::vec4 &value) const;
        void setVec4(std::string_view name, float x, float y, float z, float w) const;
        void setMat2(std::string_view name, const glm::mat2 &mat) const;
        void setMat3(std::string_view name, const glm::mat3 &mat) const;
        void setMat4(std::string_view name, const glm::mat4 &mat) const;

    private:
        void checkCompileErrors(GLuint shader, std::string_view type);
};

#endif