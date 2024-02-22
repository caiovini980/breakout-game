#include "TextRenderer.h"

#include <iostream>

#include <GLM/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../Utilities/ResourceManager.h"

TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
    // load and configure shader
    const std::string textShaderName = "text";
    const char* vertexShaderPath = "./resources/shaders/text2D.vs";
    const char* fragmentShaderPath = "./resources/shaders/text2D.frag";
    textShader = ResourceManager::LoadShader(vertexShaderPath, fragmentShaderPath, nullptr, textShaderName);
    
    textShader.SetMatrix4("projection", glm::ortho(
        0.0f,
        static_cast<float>(width),
        static_cast<float>(height),
        0.0f),
        true);
    
    // configure VAO and VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, unsigned int fontsize)
{
    characters.clear();
//
     FT_Library freeTypeLib;
     if (FT_Init_FreeType(&freeTypeLib))
     {
         std::cerr << "ERROR::FREE_TYPE: Could not init FreeType Library\n";
     }

     // load font as a "face"
     FT_Face face;
     if (FT_New_Face(freeTypeLib, font.c_str(), 0, &face))
     {
         std::cerr << "ERROR::FREE_TYPE: Failed to load a font\n";
     }

     // set size of glyphs (font items)
     FT_Set_Pixel_Sizes(face, 0, fontsize);

     // disable byte-alignment restriction
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     // for the first 128 ASCII characters, pre-load them
     for (GLubyte i = 0; i < 128; i++)
     {
         // Load the character glyph
         if (FT_Load_Char(face, i, FT_LOAD_RENDER))
         {
             std::cout << "ERROR::FREETYPE: Failed to load character glyph\n";
             continue;
         }

         // generate character texture
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

         // Store the character itself, and its components
         Character character {
             texture,
             face->glyph->advance.x,
             glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
             glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top)
         };

         characters.insert(std::pair<char, Character>(i, character));
     }

     glBindTexture(GL_TEXTURE_2D, 0);

     FT_Done_Face(face);
     FT_Done_FreeType(freeTypeLib);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    textShader.Use();
    textShader.SetVector3f("textColor", color);
    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    
    // go through all characters
    for (std::string::const_iterator i = text.begin(); i != text.end(); ++i)
    {
        const Character character = characters[*i];

        // letter offset
        const float xPosition = x + static_cast<float>(character.bearing.x) * scale;
        const float yPosition = y + static_cast<float>(characters['H'].bearing.y - character.bearing.y) * scale;
        
        const float width = static_cast<float>(character.size.x) * scale;
        const float height = static_cast<float>(character.size.y) * scale;
        
        // Update the VBO for each character
        const float vertices[6][4] = {
            { xPosition,         yPosition + height,   0.0f, 1.0f },
            { xPosition + width, yPosition,            1.0f, 0.0f },
            { xPosition,         yPosition,            0.0f, 0.0f },
        
            { xPosition,         yPosition + height,   0.0f, 1.0f },
            { xPosition + width, yPosition + height,   1.0f, 1.0f },
            { xPosition + width, yPosition,            1.0f, 0.0f }
        };
        
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, character.textureID);
        
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // render the quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // now advance cursors for next glyph
        // TODO Study bitshift operations
        x += static_cast<float>(character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // for (c = text.begin(); c != text.end(); c++)
    // {
    //     Character ch = characters[*c];
    //
    //     float xpos = x + ch.bearing.x * scale;
    //     float ypos = y + (this->characters['H'].bearing.y - ch.bearing.y) * scale;
    //
    //     float w = ch.size.x * scale;
    //     float h = ch.size.y * scale;
    //     // update VBO for each character
    //     float vertices[6][4] = {
    //         { xpos,     ypos + h,   0.0f, 1.0f },
    //         { xpos + w, ypos,       1.0f, 0.0f },
    //         { xpos,     ypos,       0.0f, 0.0f },
    //
    //         { xpos,     ypos + h,   0.0f, 1.0f },
    //         { xpos + w, ypos + h,   1.0f, 1.0f },
    //         { xpos + w, ypos,       1.0f, 0.0f }
    //     };
    //     // render glyph texture over quad
    //     glBindTexture(GL_TEXTURE_2D, ch.textureID);
    //     // update content of VBO memory
    //     glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    //     glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     // render quad
    //     glDrawArrays(GL_TRIANGLES, 0, 6);
    //     // now advance cursors for next glyph
    //     x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    // }
    // glBindVertexArray(0);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

//
// void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color)
// {
//     textShader.Use();
//     textShader.SetVector3f("textColor", color);
//
//     glActiveTexture(GL_TEXTURE0);
//     glBindVertexArray(VAO);
//
//     // go through all characters
//     for (std::string::const_iterator i = text.begin(); i != text.end(); ++i)
//     {
//         const Character character = characters[*i];
//
//         const float xPosition = x + static_cast<float>(character.bearing.x) * scale;
//         const float yPosition = y + static_cast<float>(characters['H'].bearing.y) * scale;
//
//         const float width = static_cast<float>(character.size.x) * scale;
//         const float height = static_cast<float>(character.size.y) * scale;
//
//         // Update the VBO for each character
//         const float vertices[6][4] = {
//             { xPosition,         yPosition + height,   0.0f, 1.0f },
//             { xPosition + width, yPosition,            1.0f, 0.0f },
//             { xPosition,         yPosition,            0.0f, 0.0f },
//
//             { xPosition,         yPosition + height,   0.0f, 1.0f },
//             { xPosition + width, yPosition + height,   1.0f, 1.0f },
//             { xPosition + width, yPosition,            1.0f, 0.0f }
//         };
//
//         // render glyph texture over quad
//         glBindTexture(GL_TEXTURE_2D, character.textureID);
//
//         // update content of VBO memory
//         glBindBuffer(GL_ARRAY_BUFFER, VBO);
//         glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//         // render the quad
//         glDrawArrays(GL_TRIANGLES, 0, 6);
//
//         // now advance cursors for next glyph
//         // TODO Study bitshift operations
//         x += static_cast<float>(character.advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
//
//         glBindVertexArray(0);
//         glBindTexture(GL_TEXTURE_2D, 0);
//     }
// }
