#pragma once
#include <GLAD/glad/glad.h>
#include <GLM/glm.hpp>

#include "../Utilities/Texture.h"
#include "SpriteRenderer.h"

class GameObject
{
public:
    GameObject();
    GameObject(glm::vec2 position, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f),
        glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    virtual void Draw(SpriteRenderer& renderer);
    
    Texture2D sprite;
    
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 velocity;

    glm::vec3 color;

    float rotation;
    bool isSolid;
    bool isDestroyed;
    
};
