#include "GameLevel.h"

#include <fstream>
#include <sstream>

#include "../Utilities/ResourceManager.h"

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
    this->bricks.clear();

    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fileStream(file);
    std::vector<std::vector<unsigned int>> tileData;

    if (fileStream)
    {
        while (std::getline(fileStream, line))
        {
            std::stringstream stringStream(line);
            std::vector<unsigned int> row;

            while (stringStream >> tileCode)
            {
                row.emplace_back(tileCode);
            }
            
            tileData.emplace_back(row);
        }

        if (tileData.size() > 0)
        {
            this->Init(tileData, levelWidth, levelHeight);
        }
    }
    
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
    for (GameObject& tile : this->bricks)
    {
        if (!tile.isDestroyed)
        {
            tile.Draw(renderer);
        }
    }
}

bool GameLevel::IsCompleted()
{
    for (GameObject& tile : this->bricks)
    {
        if (!tile.isSolid && !tile.isDestroyed)
        {
            return false;
        }
    }

    return true;
}

void GameLevel::CreateSolidBlock(glm::vec2 position, glm::vec2 size)
{
    std::string solidBlockTextureName = "block_solid";
    GameObject object(position, size, ResourceManager::GetTexture(solidBlockTextureName), glm::vec3(0.8f, 0.8f, 0.7f));
                
    object.isSolid = true;
    this->bricks.emplace_back(object);
}

void GameLevel::CreateNormalBlock(std::vector<std::vector<unsigned>>& tileData, unsigned y, unsigned x, glm::vec2 position, glm::vec2 size)
{
    glm::vec3 color = glm::vec3(1.0f); // original: white
    if (tileData[y][x] == 2)
    {
        color = glm::vec3(0.2f, 0.6f, 1.0f);
    }
                    
    else if (tileData[y][x] == 3)
    {
        color = glm::vec3(0.0f, 0.7f, 0.0f);
    }
                    
    else if (tileData[y][x] == 4)
    {
        color = glm::vec3(0.8f, 0.8f, 0.4f);
    }
                    
    else if (tileData[y][x] == 5)
    {
        color = glm::vec3(1.0f, 0.5f, 0.0f);
    }
                
    std::string blockTextureName = "block";
    GameObject object(position, size, ResourceManager::GetTexture(blockTextureName), color);
                
    object.isSolid = false;
    this->bricks.emplace_back(object);
}

void GameLevel::Init(std::vector<std::vector<unsigned>>& tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // calculate dimensions
    const unsigned int height = static_cast<unsigned>(tileData.size());
    const unsigned int width = static_cast<unsigned>(tileData[0].size());

    const float unitWidth = static_cast<float>(levelWidth) / static_cast<float>(width);
    const float unitHeight = static_cast<float>(levelHeight) / static_cast<float>(height);

    // initialize level tiles based on tileData matrix
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            if (tileData[y][x] == static_cast<int>(BlockTypes::NO_BLOCK)) continue;
            
            glm::vec2 position(static_cast<float>(x) * unitWidth, static_cast<float>(y) * unitHeight);
            glm::vec2 size(unitWidth, unitHeight);
            
            // check block type from level data (2D array)
            if (tileData[y][x] == static_cast<int>(BlockTypes::SOLID)) 
            {
                CreateSolidBlock(position, size);
                continue;
            }
            
            CreateNormalBlock(tileData, y, x, position, size);
        }
    }
}
