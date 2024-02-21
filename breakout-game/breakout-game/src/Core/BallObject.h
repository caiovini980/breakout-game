#pragma once
#include <GLM/glm.hpp>

#include "GameObject.h"
#include "../Utilities/Texture.h"

class BallObject : public GameObject
{
public:
    float radius;
    bool stuck;

    BallObject();
    BallObject(glm::vec2 position, float radius, glm::vec2 velocity, Texture2D sprite);

    void Reset(glm::vec2 position, glm::vec2 velocity);
    
    glm::vec2 Move(float deltaTime, unsigned int windowWidth);
};
