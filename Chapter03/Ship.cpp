// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "CircleComponent.h"
#include "Game.h"
#include "Laser.h"
#include "Asteroid.h"

Ship::Ship(Game* game)
	:Actor(game)
	,mLaserCooldown(0.0f)
    ,mCircle(nullptr)
{
	// Create a sprite component
	SpriteComponent* sc = new SpriteComponent(this, 150);
	sc->SetTexture(game->GetTexture("Assets/Ship.png"));

	// Create an input component and set keys/speed
	mIC = new InputComponent(this);
	mIC->SetForwardKey(SDL_SCANCODE_W);
	mIC->SetBackKey(SDL_SCANCODE_S);
	mIC->SetClockwiseKey(SDL_SCANCODE_A);
	mIC->SetCounterClockwiseKey(SDL_SCANCODE_D);
	mIC->SetMaxForwardSpeed(300.0f);
	mIC->SetMaxAngularSpeed(Math::TwoPi);

    // Create a circle component for collision
    mCircle = new CircleComponent(this);
    mCircle->SetRadius(32.0f);
    
}

void Ship::UpdateActor(float deltaTime)
{
	mLaserCooldown -= deltaTime;
    mRespawnCooldown -= deltaTime;
    
    if (mRespawning && mRespawnCooldown < 0)
    {
        SetState(EActive);
        SetPosition(Vector2(512.0f, 384.0f));
        SetRotation(0.0f);
        mIC->SetVelocity(0.0f);
        mRespawning = false;
    }
    else if (!mRespawning && mRespawnCooldown < 0)
    {
        // Check if we intersect with an asteroid
        for (auto ast : GetGame()->GetAsteroids())
        {
            if (Intersect(*mCircle, *(ast->GetCircle())))
            {
                // Make the ship disappear for 2 seconds
                mRespawning = true;
                mRespawnCooldown = 2.0f;
                SetState(EPaused);
                break;
            }
        }
    }
}

void Ship::ActorInput(const uint8_t* keyState)
{
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// Reset laser cooldown (half second)
		mLaserCooldown = 0.5f;
	}
}
