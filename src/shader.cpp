#include "shader.h"

#include "structs.h"

#include <string>
#include <fstream>
#include <sstream>

Shader::Shader() {}

Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
{
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderstream;

		vShaderStream << "#version " << GLSL_VERSION << "\n";
		fShaderstream << "#version " << GLSL_VERSION << "\n";

		vShaderStream << vShaderFile.rdbuf();
		fShaderstream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderstream.str();
	}
	catch (ifstream::failure &e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_PROPERLY_READ: " << e.what() << endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;

	// Compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	// Compile fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	// Link compiled shaders into a program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	// Release individual shader objects now that they are linked into the program
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::deleteProgram()
{
	glDeleteProgram(ID);
}

void Shader::use() 
{
	glUseProgram(ID); 
}

GLuint Shader::getID()
{
	return ID;
}

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(std::string_view name, bool value) const
{         
	glUniform1i(glGetUniformLocation(ID, name.data()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::setInt(std::string_view name, int value) const
{ 
	glUniform1i(glGetUniformLocation(ID, name.data()), value); 
}
// ------------------------------------------------------------------------
void Shader::setFloat(std::string_view name, float value) const
{ 
	glUniform1f(glGetUniformLocation(ID, name.data()), value); 
}
// ------------------------------------------------------------------------
void Shader::setVec2(std::string_view name, const glm::vec2 &value) const
{ 
	glUniform2fv(glGetUniformLocation(ID, name.data()), 1, &value[0]); 
}
void Shader::setVec2(std::string_view name, float x, float y) const
{ 
	glUniform2f(glGetUniformLocation(ID, name.data()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::setVec3(std::string_view name, const glm::vec3 &value) const
{ 
	glUniform3fv(glGetUniformLocation(ID, name.data()), 1, &value[0]); 
}
void Shader::setVec3(std::string_view name, float x, float y, float z) const
{ 
	glUniform3f(glGetUniformLocation(ID, name.data()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::setVec4(std::string_view name, const glm::vec4 &value) const
{ 
	glUniform4fv(glGetUniformLocation(ID, name.data()), 1, &value[0]); 
}
void Shader::setVec4(std::string_view name, float x, float y, float z, float w) const
{ 
	glUniform4f(glGetUniformLocation(ID, name.data()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::setMat2(std::string_view name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void Shader::setMat3(std::string_view name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void Shader::setMat4(std::string_view name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}




void Shader::checkCompileErrors(GLuint shader, std::string_view type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}