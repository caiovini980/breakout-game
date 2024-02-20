#pragma once

#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>

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
    
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    // Setters
    void SetKeyByIndex(int index, bool value) { keys[index] = value; }
    
private:
    GameState state;	
    bool keys[1024];
    unsigned int width, height;
};