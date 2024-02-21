#include "Game.h"

#include <iostream>

#include "../Utilities/ResourceManager.h"


Game::Game(unsigned width, unsigned height)
	: state(GameState::GAME_ACTIVE), keys(), width(width), height(height)
{ }

Game::~Game()
{ }

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
	
	Renderer = std::make_shared<SpriteRenderer>(newShader);

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
	this->level = 0;

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
}

void Game::Update(float deltaTime)
{
	Ball->Move(deltaTime, this->width);
	this->DoCollisions();

	if (Ball->position.y > static_cast<float>(this->height))
	{
		this->ResetLevel();
		this->ResetPlayer();
	}
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

		// update level visuals
		this->levels[this->level].Draw(*Renderer);

		// update player visuals
		Player->Draw(*Renderer);

		// update ball visuals
		Ball->Draw(*Renderer);
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
				box.isDestroyed = true;
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
	}
}

void Game::ResetLevel()
{
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
