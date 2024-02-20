#include "shader.h"

#include <iostream>

Shader& Shader::Use()
{
    GLCall(glUseProgram(program))
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
    GLCall(const unsigned int sVertex = glCreateShader(GL_VERTEX_SHADER))
    GLCall(glShaderSource(sVertex, 1, &vertexSource, nullptr))
    GLCall(glCompileShader(sVertex))
    CheckCompileErrors(sVertex, vertexType);
    //
    GLCall(const unsigned int sFragment = glCreateShader(GL_FRAGMENT_SHADER))
    GLCall(glShaderSource(sFragment, 1, &fragmentSource, nullptr))
    GLCall(glCompileShader(sFragment))
    CheckCompileErrors(sFragment, fragmentType);
    //
    if (geometrySource != nullptr)
    {
        GLCall(gShader = glCreateShader(GL_GEOMETRY_SHADER))
        GLCall(glShaderSource(gShader, 1, &geometrySource, nullptr))
        GLCall(glCompileShader(gShader))
        CheckCompileErrors(gShader, geometryType);
    }
    //
    // // Attach shaders to program
    GLCall(program = glCreateProgram())
    GLCall(glAttachShader(program, sVertex))
    GLCall(glAttachShader(program, sFragment))
    
    if (geometrySource != nullptr)
    {
        GLCall(glAttachShader(program, gShader))
    }
    //
    // // Link program
    GLCall(glLinkProgram(program))
    CheckCompileErrors(program, programType);
    
    GLCall(glValidateProgram(program))
    //
    // // Delete shader
    GLCall(glDeleteShader(sVertex))
    GLCall(glDeleteShader(sFragment))
    
    if (geometrySource != nullptr)
    {
        GLCall(glDeleteShader(gShader))
    }

    std::cout << "Shader Compiled!\n";
}

void Shader::SetFloat(const char* name, float value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }
        
    GLCall(glUniform1f(glGetUniformLocation(this->program, name), value))
}
void Shader::SetInteger(const char* name, int value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniform1i(glGetUniformLocation(this->program, name), value))
}
void Shader::SetVector2f(const char* name, float x, float y, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniform2f(glGetUniformLocation(this->program, name), x, y))
}
void Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniform2f(glGetUniformLocation(this->program, name), value.x, value.y))
}
void Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniform3f(glGetUniformLocation(this->program, name), x, y, z))
}
void Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniform3f(glGetUniformLocation(this->program, name), value.x, value.y, value.z))
}
void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniform4f(glGetUniformLocation(this->program, name), x, y, z, w))
}
void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniform4f(glGetUniformLocation(this->program, name), value.x, value.y, value.z, value.w))
}
void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
{
    if (useShader)
    {
        this->Use();
    }

    GLCall(glUniformMatrix4fv(glGetUniformLocation(this->program, name), 1, false, glm::value_ptr(matrix)))
}

void Shader::CheckCompileErrors(unsigned int object, const std::string& type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        GLCall(glGetShaderiv(object, GL_COMPILE_STATUS, &success))
        if (!success)
        {
            GLCall(glGetShaderInfoLog(object, 1024, nullptr, infoLog))
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
    else
    {
        GLCall(glGetProgramiv(object, GL_LINK_STATUS, &success))
        if (!success)
        {
            GLCall(glGetProgramInfoLog(object, 1024, nullptr, infoLog))
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- \n";
        }
    }
}