#pragma once
#include <vector>

#include "GameObject.h"

enum class BlockTypes
{
    NO_BLOCK = 0,
    SOLID,
    WHITE,
    COLOR1,
    COLOR2,
    COLOR3,
    COLOR4
};

class GameLevel
{
public:
    std::vector<GameObject> bricks;

    GameLevel() = default;

    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
    void Draw(SpriteRenderer& renderer);

    bool IsCompleted();

private:
    void Init(std::vector<std::vector<unsigned int>>& tileData, unsigned int levelWidth, unsigned int levelHeight);
    
    void CreateSolidBlock(glm::vec2 position, glm::vec2 size);
    void CreateNormalBlock(std::vector<std::vector<unsigned>>& tileData, unsigned y, unsigned x, glm::vec2 position, glm::vec2 size);
};
