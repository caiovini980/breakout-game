#pragma once

#include <GLAD/glad/glad.h>

class Texture2D
{
public:
    unsigned int id;
    
    Texture2D();

    void Generate(int width, int height, const unsigned char* data);
    void Bind() const;

    // Getters
    int GetInternalFormat() const { return internalFormat; }
    unsigned int GetImageFormat() const { return imageFormat; }
    
    int GetWrapS() const { return wrapS; }
    int GetWrapT() const { return wrapT; }
    int GetFilterMin() const { return filterMin; }
    int GetFilterMax() const { return filterMax; }

    // Setters
    void SetInternalFormat(int value) { internalFormat = value; }
    void SetImageFormat(unsigned int value) { imageFormat = value; }
    
    void SetWrapS(int value) { wrapS = value; }
    void SetWrapT(int value) { wrapT = value; }
    void SetFilterMin(int value) { filterMin = value; }
    void SetFilterMax(int value) { filterMax = value; }
    
private:
    unsigned int width;
    unsigned int height;

    int internalFormat; // format of texture object
    unsigned int imageFormat; // format of loaded image

    int wrapS; // wrapping mode on S axis
    int wrapT; // wrapping mode on T axis
    int filterMin; // filtering mode if texture pixels < screen pixels
    int filterMax; // filtering mode if texture pixels > screen pixels
};
