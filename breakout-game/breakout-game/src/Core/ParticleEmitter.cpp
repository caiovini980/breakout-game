#include "ParticleEmitter.h"


ParticleEmitter::ParticleEmitter(const Shader& shader, const Texture2D& texture, unsigned amount)
    : shader(shader), texture(texture), numberOfParticles(amount)
{
    Init();
}

void ParticleEmitter::Update(float deltaTime, GameObject& gameObject, unsigned newParticles, glm::vec2 offset)
{
    // add new particles
    for (unsigned int i = 0; i < newParticles; i++)
    {
        const unsigned int unusedParticle = FirstUnusedParticle();
        RespawnParticle(particles[unusedParticle], gameObject, offset);
    }

    // update all particles
    for (unsigned int i = 0; i < numberOfParticles; i++)
    {
        Particle& particle = particles[i];
        particle.Life -= deltaTime;

        if (particle.Life > 0.0f)
        {
            particle.Position -= particle.Velocity * deltaTime;
            particle.Color -= deltaTime * 2.5f;
        }
    }
}

void ParticleEmitter::Draw()
{
    // glow effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    shader.Use();

    for (Particle particle : particles)
    {
        if (particle.Life > 0.0f)
        {
            shader.SetVector2f("offset", particle.Position);
            shader.SetVector4f("color", particle.Color);
            texture.Bind();
            
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleEmitter::Init()
{
    unsigned int VBO;
    float particleQuad[]
    {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindVertexArray(0);

    for (unsigned int i = 0; i < numberOfParticles; i++)
    {
        Particle newParticle {};
        particles.emplace_back(newParticle);
    }
}

void ParticleEmitter::RespawnParticle(Particle& particle, GameObject& gameObject, glm::vec2 offset)
{
    const float random = ((rand() % 100) - 50) / 10;
    const float randomColor = 0.5f + (rand() % 100) / 100;

    particle.Position = gameObject.position + random + offset;
    particle.Color = glm::vec4(randomColor, randomColor, randomColor, 1.0f);
    particle.Life = 1.0f;
    particle.Velocity = gameObject.velocity * 0.1f;
}

unsigned ParticleEmitter::FirstUnusedParticle()
{
    for (unsigned int i = lastUsedParticle; i < numberOfParticles; i++)
    {
        if (particles[i].Life <= 0.0f)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    lastUsedParticle = 0;
    return 0;
}
