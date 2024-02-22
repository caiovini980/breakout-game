#include "Game.h"

#include <iostream>
#include <sstream>
#include <windows.h>

#include "../Utilities/ResourceManager.h"


Game::Game(unsigned width, unsigned height)
	: state(GameState::GAME_MENU), keys(), width(width), height(height)
{ }

Game::~Game()
{
	delete SoundEngine;
}

void Game::CreateShaders(const std::string& particleShaderName, const std::string& spriteShaderName, const glm::mat4 projection)
{
	// Load sprite shader
	Shader spriteShader = ResourceManager::LoadShader("./resources/shaders/sprite.vs",
		"./resources/shaders/sprite.frag", nullptr, spriteShaderName);
	spriteShader.Use();
	spriteShader.SetInteger("image", 0);
	spriteShader.SetMatrix4("projection", projection);
	
	Renderer = std::make_shared<SpriteRenderer>(spriteShader);
	
	// Load particles shader
	Shader particleShader = ResourceManager::LoadShader("./resources/shaders/particle.vs",
	                                                    "./resources/shaders/particle.frag", nullptr, particleShaderName);
	particleShader.Use();
	particleShader.SetInteger("sprite", 0);
	particleShader.SetMatrix4("projection", projection);

	// load particles texture
	std::string particleTextureName = "particle";
	ResourceManager::LoadTexture("./resources/textures/particle.png", true, particleTextureName);
	
	Particles = std::make_shared<ParticleEmitter>(
		ResourceManager::GetShader(particleShaderName),
		ResourceManager::GetTexture(particleTextureName),
		500);
}

void Game::Init()
{
    std::cout << "Starting a brand new game...\n";
	
	const std::string particleShaderName = "particle";
	const std::string spriteShaderName = "sprite";
	const std::string solidBlockTextureName = "block_solid";
	const std::string blockTextureName = "block";
	
	const glm::mat4 projection = glm::ortho(
		0.0f,
		static_cast<float>(width),
		static_cast<float>(height),
		0.0f,
		-1.0f, 1.0f);
	
	CreateShaders(particleShaderName, spriteShaderName, projection);

	// load textures
	// TODO Remove string copies
    ResourceManager::LoadTexture("./resources/textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("./resources/textures/block_solid.png", false, solidBlockTextureName);
	ResourceManager::LoadTexture("./resources/textures/block.png", false, blockTextureName);
	ResourceManager::LoadTexture("./resources/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("./resources/textures/awesomeface.png", true, "ball");

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
	this->level = 3;

	// load player
    const std::string playerTextureName = "paddle";
    const glm::vec2 playerInitialPosition = glm::vec2(
		(static_cast<float>(this->width) / 2.0f) - (PLAYER_SIZE.x / 2.0f),
		static_cast<float>(this->height) - PLAYER_SIZE.y);

    const Texture2D playerSprite = ResourceManager::GetTexture(playerTextureName);
	Player = std::make_shared<GameObject>(playerInitialPosition, PLAYER_SIZE, playerSprite);

	// load ball
	const Texture2D ballTexture = ResourceManager::GetTexture("ball");
	const glm::vec2 ballPosition = playerInitialPosition + glm::vec2(
		PLAYER_SIZE.x / 2 - BALL_RADIUS,
		-BALL_RADIUS * 2.0f);
	Ball = std::make_shared<BallObject>(ballPosition, BALL_RADIUS, INITIAL_BALL_VELOCITY, ballTexture);

	textRenderer = std::make_shared<TextRenderer>(this->width, this->height);
	textRenderer->Load("./resources/fonts/ARCADECLASSIC.TTF", 30);
	
	// load background audio
	SoundEngine->play2D("./resources/audio/breakout.mp3", true);
}

void Game::ProcessInput(float deltaTime)
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

				if (Ball->stuck)
				{
					Ball->position.x -= velocity;
				}
			}
		}

		if (this->keys[GLFW_KEY_D])
		{
			if (Player->position.x <= castedWidth - Player->size.x)
			{
				Player->position.x += velocity;
				
				if (Ball->stuck)
				{
					Ball->position.x += velocity;
				}
			}
		}

		if (this->keys[GLFW_KEY_W])
		{
			if (Player->position.y >= 0)
			{
				std::cout << "Go Up!\n";
				Player->position.y -= velocity;
				
				if (Ball->stuck)
				{
					Ball->position.y -= velocity;
				}
			}
		}

		if (this->keys[GLFW_KEY_S])
		{
			if (Player->position.y <= castedHeight - Player->size.y)
			{
				std::cout << "Go Down!\n";
				Player->position.y += velocity;
				
				if (Ball->stuck)
				{
					Ball->position.y += velocity;
				}
			}
		}

		if (this->keys[GLFW_KEY_SPACE])
		{
			Ball->stuck = false;
		}
	}

	if (state == GameState::GAME_MENU)
	{
		if (keys[GLFW_KEY_ENTER] && !keysProcessed[GLFW_KEY_ENTER])
		{
			state = GameState::GAME_ACTIVE;
			keysProcessed[GLFW_KEY_ENTER] = true;
		}

		if (keys[GLFW_KEY_W] && !keysProcessed[GLFW_KEY_W])
		{
			level = (level + 1) % 4;
			keysProcessed[GLFW_KEY_W] = true;
		}

		if (keys[GLFW_KEY_S] && !keysProcessed[GLFW_KEY_S])
		{
			if (level > 0)
			{
				level--;
			}
			else
			{
				level = 3;
			}

			keysProcessed[GLFW_KEY_S] = true;
		}
	}
}

void Game::Update(float deltaTime)
{
	Ball->Move(deltaTime, this->width);
	this->DoCollisions();

	if (Ball->position.y > static_cast<float>(this->height))
	{
		currentLives--;

		if (currentLives <= 0)
		{
			this->ResetLevel();
			state = GameState::GAME_MENU;
		}
		
		this->ResetPlayer();
	}

	Particles->Update(deltaTime, *Ball, 2, glm::vec2(Ball->radius / 2));
}

void Game::Render()
{
	if (this->state == GameState::GAME_ACTIVE || state == GameState::GAME_MENU)
	{
		// Show background
		Texture2D backgroundTexture = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(
			backgroundTexture,
			glm::vec2(0.0f, 0.0f),
			glm::vec2(this->width, this->height),
			0.0f);

		// update level visuals
		this->levels[this->level].Draw(*Renderer);

		// update player visuals
		Player->Draw(*Renderer);

		// update particles visuals
		Particles->Draw();
		
		// update ball visuals
		Ball->Draw(*Renderer);
		
		// Show GUI
		std::stringstream stringStream;
		stringStream << currentLives;
		textRenderer->RenderText("Lives    " + stringStream.str(), 10.0f, 5.0f, 1.0f);
	}

	if (state == GameState::GAME_MENU)
	{
		textRenderer->RenderText("Press    ENTER    to    start",
			250.0f, static_cast<float>(height / 2) + 20.0f, 1.0f);
		
        textRenderer->RenderText("Press    W    or    S    to    select    level",
        	245.0f, static_cast<float>(height / 2) + 50.0f, 0.75f);
	}
}

Direction Game::VectorDirection(glm::vec2 target) const
{
	constexpr glm::vec2 compass[]
	{
		glm::vec2(0.0f, 1.0f),  // UP
		glm::vec2(1.0f, 0.0f),  // RIGHT
		glm::vec2(0.0f, -1.0f),  // DOWN
		glm::vec2(-1.0f, 0.0f),  // LEFT
	};

	float max = 0.0f;
	unsigned int bestMatch = -1;

	for (unsigned int i = 0; i < 4; i++)
	{
		const float dotProduct = glm::dot(glm::normalize(target), compass[i]);
		
		if (dotProduct > max)
		{
			max = dotProduct;
			bestMatch = i;
		}
	}

	return static_cast<Direction>(bestMatch);
}

// TODO Make a collision class for those collision methods
void Game::DoCollisions()
{
	// block collisions
	for (GameObject& box : this->levels[this->level].bricks)
	{
		if (box.isDestroyed) continue;

		Collision collision = CheckCollisions(*Ball, box);
		if (std::get<0>(collision))
		{
			if (!box.isSolid)
			{
				// load non-solid block hit audio
				SoundEngine->play2D("./resources/audio/bleep.mp3", false);
				box.isDestroyed = true;
			}

			if (box.isSolid)
			{
				// load solid block hit audio
				SoundEngine->play2D("./resources/audio/solid.wav", false);
			}

			const Direction direction = std::get<1>(collision);
			const glm::vec2 distance = std::get<2>(collision);

			// horizontal collision
			if (direction == Direction::LEFT || direction == Direction::RIGHT)
			{
				Ball->velocity.x = -Ball->velocity.x;

				const float penetration = Ball->radius - std::abs(distance.x);

				if (direction == Direction::LEFT)
				{
					Ball->position.x += penetration;
				}
				else
				{
					Ball->position.x -= penetration;
				}
			}
			else
			{
				Ball->velocity.y = -Ball->velocity.y;

				const float penetration = Ball->radius - std::abs(distance.y);

				if (direction == Direction::UP)
				{
					Ball->position.y -= penetration;
				}
				else
				{
					Ball->position.y += penetration;
				}
			}
			
		}
	}

	// paddle collision
	const Collision result = CheckCollisions(*Ball, *Player);
	if (!Ball->stuck && std::get<0>(result))
	{
		const float centerPaddle = Player->position.x + (Player->size.x / 2);
		const float distanceFromCenter = (Ball->position.x + Ball->radius) - centerPaddle;
		const float percentage = distanceFromCenter / (Player->size.x / 2);
		constexpr float strength = 2.0f;

		const glm::vec2 oldVelocity = Ball->velocity;
		Ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->velocity.y = -1 * abs(Ball->velocity.y);

		Ball->velocity = glm::normalize(Ball->velocity) * glm::length(oldVelocity);
		
		// load paddle hit audio
		SoundEngine->play2D("./resources/audio/bleep.wav", false);
	}
}

void Game::ResetLevel()
{
	currentLives = INITIAL_AMOUNT_OF_LIVES;
	
	switch (level)
	{
	case 0:
		levels[0].Load("./resources/levels/Level1.txt", this->width, this->height / 2);
		break;
		
	case 1:
		levels[1].Load("./resources/levels/Level2.txt", this->width, this->height / 2);
		break;

	case 2:
		levels[2].Load("./resources/levels/Level3.txt", this->width, this->height / 2);
		break;

	case 3:
		levels[3].Load("./resources/levels/Level4.txt", this->width, this->height / 2);
		break;

	default:
		levels[0].Load("./resources/levels/Level1.txt", this->width, this->height / 2);
		break;		
	}
}

void Game::ResetPlayer()
{
	Player->size = PLAYER_SIZE;
	Player->position = glm::vec2(
		(static_cast<float>(this->width) / 2.0f) - (PLAYER_SIZE.x / 2.0f),
		static_cast<float>(this->height) - PLAYER_SIZE.y);
	Ball->Reset(Player->position + glm::vec2(
		PLAYER_SIZE.x / 2 - BALL_RADIUS,
		-BALL_RADIUS * 2.0f), INITIAL_BALL_VELOCITY);
}

bool Game::CheckCollisions(const GameObject& A, const GameObject& B) const
{
	const float wholeXA = A.position.x + A.size.x;
	const float wholeXB = B.position.x + B.size.x;
	const bool collisionX = wholeXA >= B.position.x && wholeXB >= A.position.x;

	const float wholeYA = A.position.y + A.size.y;
	const float wholeYB = B.position.y + B.size.y;
	const bool collisionY = wholeYA >= B.position.y && wholeYB >= A.position.y;

	return collisionX && collisionY;
}

Collision Game::CheckCollisions(const BallObject& ball, const GameObject& other) const
{
	const glm::vec2 otherHalfExtents = glm::vec2(other.size.x / 2, other.size.y / 2);
	const glm::vec2 ballCenter = ball.position + ball.radius;
	const glm::vec2 otherCenter = glm::vec2(					// this happens because the origin point is (0, 0)
		other.position.x + otherHalfExtents.x,
		other.position.y + otherHalfExtents.y);

	glm::vec2 distanceBetweenCenters = ballCenter - otherCenter;
	
	const glm::vec2 clampedDistance = glm::clamp(distanceBetweenCenters, - otherHalfExtents, otherHalfExtents);
	const glm::vec2 closestPoint = otherCenter + clampedDistance;

	distanceBetweenCenters = closestPoint - ballCenter;
	if (glm::length(distanceBetweenCenters) < ball.radius)
	{
		return std::make_tuple(true, VectorDirection(distanceBetweenCenters), distanceBetweenCenters);
	}
	
	return std::make_tuple(false, Direction::UP, glm::vec2(0.0f, 0.0f));
}
