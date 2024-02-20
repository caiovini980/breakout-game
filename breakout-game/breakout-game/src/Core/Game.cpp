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
	const std::string textureName = "face";
	
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

	ResourceManager::LoadTexture("./resources/textures/MiiSemFundo.png", true, textureName);
}

void Game::ProcessInput(float dt)
{
}

void Game::Update(float dt)
{
}

void Game::Render()
{
	const std::string textureName = "face";
	Texture2D texture = ResourceManager::GetTexture(textureName);
	Renderer->DrawSprite(
		texture,
		glm::vec2(0.0f, 0.0f),
		glm::vec2(200.0f, 300.0f),
		0.0f,
		glm::vec3(0.0f, 1.0f, 1.0f));
}
