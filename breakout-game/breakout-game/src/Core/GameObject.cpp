#include "GameObject.h"

GameObject::GameObject()
    : sprite(),position(0.0f, 0.0f), size(1.0f, 1.0f), velocity(0.0f), color(1.0f), rotation(0.0f), isSolid(false), isDestroyed(false)
{
}

GameObject::GameObject(glm::vec2 position, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : sprite(sprite),position(position), size(size), velocity(velocity), color(color), rotation(0.0f), isSolid(false), isDestroyed(false)
{
}

void GameObject::Draw(SpriteRenderer& renderer)
{
    renderer.DrawSprite(this->sprite, this->position, this->size, this->rotation, this->color);
}
