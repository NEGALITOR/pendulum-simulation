#include <ft2build.h>
#include FT_FREETYPE_H


#include "globals.h"
#include "structs.h"
#include "textRender.h"

void TextRender::initTextRenderShaders(int fontSize) 
{
    Shader textRenderShader("shaders/textRenderVShader.glsl", "shaders/textRenderFShader.glsl");

    
    // Allocate a VAO and dynamic VBO for streaming per-character quad vertices
    VAO.generate();
    VBO.generate();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    shaders.push_back(textRenderShader);

    loadFont(fontSize);
    generateFontTextures();

    

}


void TextRender::generateFontTextures()
{
    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Rasterize and upload a greyscale glyph texture for each printable ASCII character
    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph 
        if (FT_Load_Char(face.get(), c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR: Failed to load Glyph" << std::endl;
            continue;
        }
        
        // Generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Store character for later use
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
}

void TextRender::loadFont(int fontSize)
{
    // Initialize FreeType
    FT_Library rawFt = nullptr;
    if (FT_Init_FreeType(&rawFt)) {
        std::cerr << "Could not initialize FreeType library." << std::endl;
        exit(1);
    }
    ft.reset(rawFt);

    // Load font face
    FT_Face rawFace = nullptr;
    if (FT_New_Face(ft.get(), "fonts/UniversCondensed.ttf", 0, &rawFace)) {
        std::cerr << "Could not load font." << std::endl;
        exit(1);
    }
    face.reset(rawFace);

    // Set font size
    FT_Set_Pixel_Sizes(face.get(), 0, fontSize); // Adjust the size to your needs
}


void TextRender::renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 textColor) 
{
    
   
    // Get the current window size
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window.get(), &windowWidth, &windowHeight);

    // Define the plot area in the lower right corner (1/4 of the window)
    float plotWidth = windowWidth / 2;
    float plotHeight = windowHeight / 2;
    int plotX = 10;  // 10 pixel margin
    int plotY = 10;  // 10 pixel margin from bottom
    
    // Save current viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glViewport(plotX, plotY, plotWidth, plotHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    shader.use();

    // Set up orthographic projection
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    shaders[3].setMat4("projection", projection);
    /*
    // Draw plot background
    shader.setVec4("textColor", glm::vec4(0.1f, 0.1f, 0.1f, 0.7f));
    
    const float thetaRange = 2.0f * M_PI;
    const float thetaDotRange = 20.0f;
    float backgroundVertices[] = {
        0.0f, 0.0f,
        plotWidth, 0.0f,
        plotWidth, plotHeight,
        0.0f, plotHeight
    };
    
    GLuint backgroundVAO, backgroundVBO;
    glGenVertexArrays(1, &backgroundVAO);
    glGenBuffers(1, &backgroundVBO);
    
    glBindVertexArray(backgroundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(backgroundVertices), backgroundVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDeleteVertexArrays(1, &backgroundVAO);
    glDeleteBuffers(1, &backgroundVBO);
    */
    
    shader.setVec3("textColor", textColor);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);


    

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        

        //std::cout << "Rendering '" << *c << "' at (" << xpos << "," << ypos << ") with size " 
        //      << w << "x" << h << std::endl;

        // Build a screen-space quad for this glyph using two triangles
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },  // Top-left
            { xpos,     ypos,       0.0f, 1.0f },  // Bottom-left
            { xpos + w, ypos,       1.0f, 1.0f },  // Bottom-right
                    
            { xpos,     ypos + h,   0.0f, 0.0f },  // Top-left
            { xpos + w, ypos,       1.0f, 1.0f },  // Bottom-right
            { xpos + w, ypos + h,   1.0f, 0.0f }   // Top-right
        };
        
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Advance position for next glyph
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to convert 1/64th pixels to pixels
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore original viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glDisable(GL_BLEND);
}