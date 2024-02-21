﻿#include "Game.h"

#include <iostream>

#include "../Utilities/ResourceManager.h"


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
	// TODO Remove string copies
    ResourceManager::LoadTexture("./resources/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("./resources/textures/block_solid.png", false, solidBlockTextureName);
	ResourceManager::LoadTexture("./resources/textures/block.png", false, blockTextureName);
	ResourceManager::LoadTexture("./resources/textures/paddle.png", true, "paddle");

	// load levels
	// TODO create a proper level loader method/class
	GameLevel level1;
	GameLevel level2;
	GameLevel level3;
	GameLevel level4;
	
	level1.Load("./resources/levels/Level1.txt", this->width, this->height / 2);
	level2.Load("./resources/levels/Level2.txt", this->width, this->height / 2);
	level3.Load("./resources/levels/Level3.txt", this->width, this->height / 2);
	level4.Load("./resources/levels/Level4.txt", this->width, this->height / 2);
	
	this->levels.emplace_back(level1);
	this->levels.emplace_back(level2);
	this->levels.emplace_back(level3);
	this->levels.emplace_back(level4);
	this->level = 0;

	// load player
    const std::string playerTextureName = "paddle";
    const glm::vec2 playerInitialPosition = glm::vec2(
		(static_cast<float>(this->width) / 2.0f) - (PLAYER_SIZE.x / 2.0f),
		static_cast<float>(this->height) - PLAYER_SIZE.y);

    const Texture2D playerSprite = ResourceManager::GetTexture(playerTextureName);
	Player = new GameObject(playerInitialPosition, PLAYER_SIZE, playerSprite);
}

void Game::ProcessInput(float deltaTime) const
{
	if (this->state == GameState::GAME_ACTIVE)
	{
		const float velocity = PLAYER_VELOCITY * deltaTime;
		const float castedWidth = static_cast<float>(this->width);
		const float castedHeight= static_cast<float>(this->height);

		if (this->keys[GLFW_KEY_A])
		{
			if (Player->position.x >= 0.0f)
			{
				Player->position.x -= velocity;
			}
		}

		if (this->keys[GLFW_KEY_D])
		{
			if (Player->position.x <= castedWidth - Player->size.x)
			{
				Player->position.x += velocity;
			}
		}

		if (this->keys[GLFW_KEY_W])
		{
			if (Player->position.y >= 0)
			{
				Player->position.y -= velocity;
				std::cout << "Go Up!\n";
			}
		}

		if (this->keys[GLFW_KEY_S])
		{
			if (Player->position.y <= castedHeight - Player->size.y)
			{
				std::cout << "Go Down!\n";
				Player->position.y += velocity;
			}
		}
	}
}

void Game::Update(float deltaTime)
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

		Player->Draw(*Renderer);
	}
}
