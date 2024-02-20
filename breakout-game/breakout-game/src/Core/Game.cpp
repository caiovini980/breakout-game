#include "Game.h"

Game::Game(unsigned width, unsigned height)
	: state(GameState::GAME_ACTIVE), keys(), width(width), height(height)
{
}

Game::~Game()
{
}

void Game::Init()
{
}

void Game::ProcessInput(float dt)
{
}

void Game::Update(float dt)
{
}

void Game::Render()
{
}
