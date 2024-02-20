#pragma once
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
x;\
ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// void GLClearError();
// bool GLLogCall(const char* function, const char* file, int line);

inline void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR]: (" << error << ")\nAt function: " << function << "\nAt file: " << file << "\nAt line: " << line << "\n";
        return false;
    }

    return true;
}
