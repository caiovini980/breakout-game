#pragma once
#include <vector>
#include <GLM/glm.hpp>

#include "GameObject.h"
#include "../Utilities/shader.h"

struct Particle
{
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec4 Color;
    float Life;

    Particle()
        : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleEmitter
{
public:
    ParticleEmitter(const Shader& shader, const Texture2D& texture, unsigned int amount);
    void Update(float deltaTime, GameObject& gameObject, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void Draw();

private:
    std::vector<Particle> particles;

    Shader shader;
    Texture2D texture;

    unsigned int numberOfParticles = 500;
    unsigned int lastUsedParticle = 0;
    unsigned int VAO;

    void Init();
    void RespawnParticle(Particle& particle, GameObject& gameObject, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

    unsigned int FirstUnusedParticle();
};
