#pragma once

#include <vector>
#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>
#include <IRRKLANG/irrKlang.h>

#include "BallObject.h"
#include "GameLevel.h"
#include "ParticleEmitter.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"

enum class GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum class Direction
{
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
};

// get collision data as a tuple
typedef std::tuple<bool, Direction, glm::vec2> Collision;

class Game
{
public:
    Game(unsigned int width, unsigned int height);
    ~Game();
    
    void Init();
    
    void ProcessInput(float deltaTime);
    void Update(float deltaTime);
    void Render();

    // Setters
    void SetKeyByIndex(int index, bool value) { keys[index] = value; }
    void SetProcessedKeyByIndex(int index, bool value) { keysProcessed[index] = value; }
    
private:
    Direction VectorDirection(glm::vec2 target) const;
    
    void DoCollisions();
    void ResetLevel();
    void ResetPlayer();
    void CreateShaders(const std::string& particleShaderName, const std::string& spriteShaderName, const glm::mat4 projection);
    
    // TODO talvez de bronca esses consts
    bool CheckCollisions(const GameObject& A, const GameObject& B) const;
    Collision CheckCollisions(const BallObject& ball, const GameObject& other) const;
    
    // TODO: Remove this from the heap
    std::shared_ptr<SpriteRenderer> Renderer;
    std::shared_ptr<GameObject> Player;
    std::shared_ptr<BallObject> Ball;
    std::shared_ptr<ParticleEmitter> Particles;
    std::shared_ptr<TextRenderer> textRenderer;

    // External libraries (raw pointers)
    irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice(); 

    // players cache
    const glm::vec2 PLAYER_SIZE = glm::vec2(100.0f, 20.0f);
    const float PLAYER_VELOCITY = 500.0f;
    const unsigned int INITIAL_AMOUNT_OF_LIVES = 3;
    unsigned int currentLives = INITIAL_AMOUNT_OF_LIVES;

    // balls cache
    const glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(100.0f, -350.0f);
    const float BALL_RADIUS = 12.5f;

    // levels cache
    std::vector<GameLevel> levels;
    unsigned int level;
    
    GameState state;
    
    bool keys[1024];
    bool keysProcessed[1024];
    
    unsigned int width;
    unsigned int height;
};