#ifndef TEXTRENDER_H
#define TEXTRENDER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "globals.h"
#include "structs.h"
#include <map>




class TextRender
{
public:

    GLuint VAO, VBO;

    FT_Library ft;
    FT_Face face;
    
    void initTextRenderShaders(int fontSize);
    void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 textColor);
    map<char, Character> Characters;

private:

    void loadFont(int fontSize);
    void generateFontTextures();

};

#endif