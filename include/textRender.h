#ifndef TEXTRENDER_H
#define TEXTRENDER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>

#include "globals.h"
#include "structs.h"
#include "glObjects.h"
#include <map>

// Custom deleters for FreeType library and face handles
struct FTLibraryDeleter {
    void operator()(FT_LibraryRec_* lib)  const { FT_Done_FreeType(lib); }
};
struct FTFaceDeleter {
    void operator()(FT_FaceRec_* face) const { FT_Done_Face(face); }
};

class TextRender
{
public:

    GLVAO    VAO;
    GLBuffer VBO;

    std::unique_ptr<FT_LibraryRec_, FTLibraryDeleter> ft;
    std::unique_ptr<FT_FaceRec_, FTFaceDeleter>    face;
    
    void initTextRenderShaders(int fontSize);
    void renderText(Shader &shader, std::string text, float x, float y, float scale, glm::vec3 textColor);
    map<char, Character> Characters;

private:

    void loadFont(int fontSize);
    void generateFontTextures();

};

#endif