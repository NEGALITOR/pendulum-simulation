#include "mesh.h"

#include "structs.h"
#include "shader.h"

#include <vector>

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;


    setupMesh();
}

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, glm::vec3 color)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh(color);
}

void Mesh::Draw(Shader &shader)
{
    /*
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;
    

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = to_string(specularNr++); // transfer unsigned int to string
        else if(name == "texture_normal")
            number = to_string(normalNr++); // transfer unsigned int to string
        else if(name == "texture_height")
            number = to_string(heightNr++); // transfer unsigned int to string

        glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);

    }*/

    if (indices.empty()) {
        std::cerr << "Error: Indices array is empty!" << std::endl;
        return;
    }
    //cout << indices.size() << endl;
    //cout << vertices.size() << endl;
    glBindVertexArray(VAO[0]);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    for (int i = 0; i < vertices.size(); i++)
    {
        //printf("%.2f, %.2f, %.2f\n", vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
    }
    for (int i = 0; i < indices.size(); i++)
    {
        //cout << indices[i] << endl;
    }


    
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);

    //glActiveTexture(GL_TEXTURE0);

}

void Mesh::setupMesh()
{
    
    vector<glm::vec3> colors = fillColors(indices.size(), glm::vec3(1.0f, 0.0f, 0.0f));

    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);

    cout << vertices.size() << endl;
    cout << indices.size() << endl;
    cout << colors.size() << endl;


    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
    

    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

    // Color
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    

    glBindVertexArray(0);
    
}

void Mesh::setupMesh(glm::vec3 color)
{
    
    vector<glm::vec3> colors = fillColors(indices.size(), color);

    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);

    cout << vertices.size() << endl;
    cout << indices.size() << endl;
    cout << colors.size() << endl;


    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
    

    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

    // Color
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    

    glBindVertexArray(0);
    
}


vector<glm::vec3> Mesh::fillColors(int numVerts, glm::vec3 color)
{
    vector<glm::vec3> colors(numVerts);

    for (unsigned int i = 0; i < numVerts; i++)
    {
        colors[i] = color;
    }
    return colors;
}
