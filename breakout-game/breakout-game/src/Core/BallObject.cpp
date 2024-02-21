#include "BallObject.h"

BallObject::BallObject()
    : GameObject(), radius(12.5f), stuck(true)
{
}

BallObject::BallObject(glm::vec2 position, float radius, glm::vec2 velocity, Texture2D sprite)
    : GameObject(position, glm::vec2(radius * 2.0f, radius * 2.0f), sprite, glm::vec3(1.0f), velocity),
        radius(radius), stuck(true)
{
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
    this->position = position;
    this->velocity = velocity;
    this->stuck = true;
}

glm::vec2 BallObject::Move(float deltaTime, unsigned windowWidth)
{
    if (this->stuck)
    {
        return this->position;
    }

    const float castedWindowWidth = static_cast<float>(windowWidth);

    this->position += this->velocity * deltaTime;

    if (this->position.x <= 0.0f)
    {
        this->velocity.x = -this->velocity.x;
        this->position.x = 0.0f;
    }

    if (this->position.x + this->size.x >= castedWindowWidth)
    {
        this->velocity.x = -this->velocity.x;
        this->position.x = castedWindowWidth - this->size.x;
    }

    if (this->position.y <= 0.0f)
    {
        this->velocity.y = -this->velocity.y;
        this->position.y = 0.0f;
    }

    return this->position;
}
