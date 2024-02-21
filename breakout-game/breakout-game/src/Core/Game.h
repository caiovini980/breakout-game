#pragma once

#include <vector>
#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>

#include "GameLevel.h"
#include "SpriteRenderer.h"

enum class GameState
{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game
{
public:
    Game(unsigned int width, unsigned int height);
    ~Game();
    
    void Init();
    
    void ProcessInput(float deltaTime) const;
    void Update(float deltaTime);
    void Render();

    // Setters
    void SetKeyByIndex(int index, bool value) { keys[index] = value; }
    
private:
    // TODO: Remove this from the heap
    SpriteRenderer* Renderer;
    GameObject* Player;

    // players cache
    const glm::vec2 PLAYER_SIZE = glm::vec2(100.0f, 20.0f);
    const float PLAYER_VELOCITY = 500.0f;

    // levels cache
    std::vector<GameLevel> levels;
    unsigned int level;
    
    GameState state;
    
    bool keys[1024];
    
    unsigned int width;
    unsigned int height;
};