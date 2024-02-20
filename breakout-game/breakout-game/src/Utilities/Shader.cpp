#include "shader.h"

#include <iostream>

Shader& Shader::Use()
{
    glUseProgram(this->id);
    return *this;
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    unsigned int gShader;
    const std::string vertexType = "VERTEX";
    const std::string fragmentType = "FRAGMENT";
    const std::string geometryType = "GEOMETRY";
    const std::string programType = "PROGRAM";
    //
    // // Create shaders
    const unsigned int sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, nullptr);
    glCompileShader(sVertex);
    CheckCompileErrors(sVertex, vertexType);
    //
    const unsigned int sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, nullptr);
    glCompileShader(sFragment);
    CheckCompileErrors(sFragment, fragmentType);
    //
    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, nullptr);
        glCompileShader(gShader);
        CheckCompileErrors(gShader, geometryType);
    }
    //
    // // Attach shaders to program
    this->id = glCreateProgram();
    glAttachShader(this->id, sVertex);
    glAttachShader(this->id, sFragment);
    
    if (geometrySource != nullptr)
    {
        glAttachShader(this->id, gShader);
    }
    //
    // // Link program to it's ID
    glLinkProgram(this->id);
    CheckCompileErrors(this->id, programType);
    //
    // // Delete shader
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    
    if (geometrySource != nullptr)
    {
        glDeleteShader(gShader);
    }
}

void Shader::SetFloat(const char* name, float value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }
        
    glUniform1f(glGetUniformLocation(this->id, name), value);
}
void Shader::SetInteger(const char* name, int value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniform1i(glGetUniformLocation(this->id, name), value);
}
void Shader::SetVector2f(const char* name, float x, float y, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniform2f(glGetUniformLocation(this->id, name), x, y);
}
void Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniform2f(glGetUniformLocation(this->id, name), value.x, value.y);
}
void Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniform3f(glGetUniformLocation(this->id, name), x, y, z);
}
void Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniform3f(glGetUniformLocation(this->id, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniform4f(glGetUniformLocation(this->id, name), x, y, z, w);
}
void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniform4f(glGetUniformLocation(this->id, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}

void Shader::CheckCompileErrors(unsigned int object, const std::string& type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, nullptr, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, nullptr, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
}