#include "mesh.h"

#include "structs.h"
#include "shader.h"

#include <vector>

/*
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, glm::vec3 color)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh(color);
}
*/
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::Draw(Shader &shader)
{
    /*if (indices.empty()) {
        std::cerr << "Error: Indices array is empty!" << std::endl;
        return;
    }*/

    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;

    //cout << indices.size() << endl;
    //cout << vertices.size() << endl;
    printf("%d\n", textures.size());
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++); // transfer unsigned int to string
        else if(name == "texture_normal")
            number = std::to_string(normalNr++); // transfer unsigned int to string
        else if(name == "texture_height")
            number = std::to_string(heightNr++); // transfer unsigned int to string
        
        
        // now set the sampler to the correct texture unit
        
        GLint location = glGetUniformLocation(shader.ID, (name + number).c_str());
        if (location == -1) {
            std::cerr << "Error: Invalid uniform location for " << (name + number) << std::endl;
            return;
        }
        glUniform1i(location, i);

        
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        
    }

    glBindVertexArray(VAO[0]);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    /*
    for (int i = 0; i < vertices.size(); i++)
    {
        //printf("%.2f, %.2f, %.2f\n", vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
    }
        
    for (int i = 0; i < indices.size(); i+=3)
    {
        cout << i << endl;
    }
    */

    
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);


}

/*
void Mesh::setupMesh(glm::vec3 color)
{

    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);

    
    //cout << vertices.size() << endl;
    //cout << indices.size() << endl;
    //cout << colors.size() << endl;
    

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
    setupColor(color);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    
    glBindVertexArray(0);
    
}
*/



void Mesh::setupMesh()
{
    /*
    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);

    
    //cout << vertices.size() << endl;
    //cout << indices.size() << endl;
    //cout << colors.size() << endl;
    

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

    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);
    
    glBindVertexArray(0);

    */

    // create buffers/arrays
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(VAO[0]);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
    

    glBindVertexArray(0);
    
}

void Mesh::setupColor(glm::vec3 color)
{
    vector<glm::vec3> colors = fillColors(indices.size(), color);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

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
