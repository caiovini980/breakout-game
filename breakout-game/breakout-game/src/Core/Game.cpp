#include "Game.h"

#include <iostream>

#include "../Utilities/ResourceManager.h"

// TODO: Remove this from the heap
SpriteRenderer* Renderer;

Game::Game(unsigned width, unsigned height)
	: state(GameState::GAME_ACTIVE), keys(), width(width), height(height)
{
	
}

Game::~Game()
{
}

void Game::Init()
{
    std::cout << "Starting a brand new game...\n";
	
	const std::string shaderName = "sprite";
	const std::string solidBlockTextureName = "block_solid";
	const std::string blockTextureName = "block";
	
	const glm::mat4 projection = glm::ortho(
		0.0f,
		static_cast<float>(width),
		static_cast<float>(height),
		0.0f,
		-1.0f, 1.0f);
	
	Shader newShader = ResourceManager::LoadShader("./resources/shaders/sprite.vs",
		"./resources/shaders/sprite.frag", nullptr, shaderName);
	newShader.Use();
	newShader.SetInteger("image", 0);
	newShader.SetMatrix4("projection", projection);
	
	Renderer = new SpriteRenderer(newShader);

	// load textures
    ResourceManager::LoadTexture("./resources/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("./resources/textures/block_solid.png", false, solidBlockTextureName);
	ResourceManager::LoadTexture("./resources/textures/block.png", false, blockTextureName);

	// load levels
	GameLevel level1;
	level1.Load("./resources/levels/level0.txt", this->width, this->height / 2);
	this->levels.emplace_back(level1);
	this->level = 0;
}

void Game::ProcessInput(float dt)
{
}

void Game::Update(float dt)
{
}

void Game::Render()
{
	if (this->state == GameState::GAME_ACTIVE)
	{
		Texture2D backgroundTexture = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(
			backgroundTexture,
			glm::vec2(0.0f, 0.0f),
			glm::vec2(this->width, this->height),
			0.0f);

		this->levels[this->level].Draw(*Renderer);
	}
}
