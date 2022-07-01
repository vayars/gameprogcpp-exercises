// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Ship.h"
#include "AnimSpriteComponent.h"
#include "Game.h"

Ship::Ship(Game* game)
	:Actor(game)
	,mRightSpeed(0.0f)
	,mDownSpeed(0.0f)
{
	// Create an animated sprite component
	AnimSpriteComponent* asc = new AnimSpriteComponent(this);
	std::vector<SDL_Texture*> anims = {
		game->GetTexture("Assets/Character01.png"),
		game->GetTexture("Assets/Character02.png"),
        game->GetTexture("Assets/Character03.png"),
        game->GetTexture("Assets/Character04.png"),
        game->GetTexture("Assets/Character05.png"),
        game->GetTexture("Assets/Character06.png"),
        game->GetTexture("Assets/Character07.png"),
        game->GetTexture("Assets/Character08.png"),
        game->GetTexture("Assets/Character09.png"),
        game->GetTexture("Assets/Character10.png"),
        game->GetTexture("Assets/Character11.png"),
        game->GetTexture("Assets/Character12.png"),
        game->GetTexture("Assets/Character13.png"),
        game->GetTexture("Assets/Character14.png"),
        game->GetTexture("Assets/Character15.png"),
        game->GetTexture("Assets/Character16.png"),
        game->GetTexture("Assets/Character17.png"),
        game->GetTexture("Assets/Character18.png"),
	};
    
    // Create the ranges for each animation
    std::vector<int> run = {1,2,3,4,5,6};
    std::vector<int> jump = {7,8,9,10,11,12,13,14,15};
    std::vector<int> punch = {16,17,18};
    
    // Create the map of animation ranges
    std::unordered_map<std::string, std::vector<int>> ranges;
    ranges.emplace("run", run);
    ranges.emplace("jump", jump);
    ranges.emplace("punch", punch);
    
    // Determine which animations loop
    std::unordered_map<std::string, bool> looping;
    looping.emplace("run", true);
    looping.emplace("jump", false);
    looping.emplace("punch", false);
    
    // Parameters: animations, ranges, looping, default animation, and starting animation
	asc->SetAnimTextures(anims, ranges, looping, "run", "jump");
}

void Ship::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);
	// Update position based on speeds and delta time
	Vector2 pos = GetPosition();
	pos.x += mRightSpeed * deltaTime;
	pos.y += mDownSpeed * deltaTime;
	// Restrict position to left half of screen
	if (pos.x < 25.0f)
	{
		pos.x = 25.0f;
	}
	else if (pos.x > 500.0f)
	{
		pos.x = 500.0f;
	}
	if (pos.y < 25.0f)
	{
		pos.y = 25.0f;
	}
	else if (pos.y > 743.0f)
	{
		pos.y = 743.0f;
	}
	SetPosition(pos);
}

void Ship::ProcessKeyboard(const uint8_t* state)
{
	mRightSpeed = 0.0f;
	mDownSpeed = 0.0f;
	// right/left
	if (state[SDL_SCANCODE_D])
	{
		mRightSpeed += 250.0f;
	}
	if (state[SDL_SCANCODE_A])
	{
		mRightSpeed -= 250.0f;
	}
	// up/down
	if (state[SDL_SCANCODE_S])
	{
		mDownSpeed += 300.0f;
	}
	if (state[SDL_SCANCODE_W])
	{
		mDownSpeed -= 300.0f;
	}
}
