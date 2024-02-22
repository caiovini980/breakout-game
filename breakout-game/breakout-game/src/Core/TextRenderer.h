#pragma once
#include <map>

#include <GLAD/glad/glad.h>
#include <GLM/glm.hpp>

#include "../Utilities/Texture.h"
#include "../Utilities/Shader.h"

/// Holds all state information relevant to a character as loaded using FreeType
// struct Character {
//     unsigned int TextureID; // ID handle of the glyph texture
//     glm::ivec2   Size;      // size of glyph
//     glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
//     unsigned int Advance;   // horizontal offset to advance to next glyph
// };

struct Character
{
    unsigned int textureID;
    unsigned int advance;   // horizontal offset to advance to next glyph

    glm::ivec2 size;        
    glm::ivec2 bearing;     // offset from baseline to left/top of glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
    // holds a list of pre-compiled Characters
    // std::map<char, Character> Characters;
    std::map<char, Character> characters;
    // shader used for text rendering
    // Shader TextShader;
    Shader textShader;
    // constructor
    TextRenderer(unsigned int width, unsigned int height);
    // pre-compiles a list of characters from the given font
    void Load(std::string font, unsigned int fontSize);
    // renders a string of text using the precompiled list of characters
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
private:
    // render state
    unsigned int VAO, VBO;
};




// struct Character
// {
//     unsigned int textureID;
//     unsigned int advance;   // horizontal offset to advance to next glyph
//
//     glm::ivec2 size;        
//     glm::ivec2 bearing;     // offset from baseline to left/top of glyph
// };
//
// class TextRenderer
// {
// public:
//     std::map<char, Character> characters;
//
//     Shader textShader;
//
//     TextRenderer(unsigned int width, unsigned int height);
//
//     void Load(const std::string& font, unsigned int fontSize);
//     void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color = glm::vec3(1.0f));
//
// private:
//     unsigned int VAO;
//     unsigned int VBO;
// };
