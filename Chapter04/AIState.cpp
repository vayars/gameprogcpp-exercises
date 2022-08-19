// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AIState.h"
#include "AIComponent.h"
#include "Enemy.h"
#include "Actor.h"
#include "Game.h"
#include "Bullet.h"
#include <SDL/SDL_log.h>

void AIPatrol::Update(float deltaTime)
{
	SDL_Log("Updating %s state", GetName());
    
    // Check if there is an enemy
    Actor* owner = mOwner->GetActor();
    Enemy* e = owner->GetGame()->GetNearestEnemy(owner->GetPosition());
    if (e != nullptr)
    {
        // Vector from me to enemy
        Vector2 dir = e->GetPosition() - owner->GetPosition();
        float dist = dir.Length();
        // If the enemy is nearby, change to Attack state
        if (dist < AttackRange)
        {
            mOwner->ChangeState("Attack");
        }
    }
	/*bool dead = true;
	if (dead)
	{
		mOwner->ChangeState("Death");
	}*/
}

void AIPatrol::OnEnter()
{
	SDL_Log("Entering %s state", GetName());
}

void AIPatrol::OnExit()
{
	SDL_Log("Exiting %s state", GetName());
}

void AIDeath::Update(float deltaTime)
{
	SDL_Log("Updating %s state", GetName());
}

void AIDeath::OnEnter()
{
	SDL_Log("Entering %s state", GetName());
}

void AIDeath::OnExit()
{
	SDL_Log("Exiting %s state", GetName());
}

void AIAttack::Update(float deltaTime)
{
	SDL_Log("Updating %s state", GetName());
    
    // Check for an enemy nearby
    Actor* owner = mOwner->GetActor();
    Enemy* e = owner->GetGame()->GetNearestEnemy(owner->GetPosition());
    
    // Update cooldown
    mNextAttack -= deltaTime;
    
    // If cooldown = 0 and there's an enemy still in range, fire a bullet
    if (mNextAttack <= 0.0f && e != nullptr)
    {
        SDL_Log("Ready to attack");
        // Vector from me to enemy
        Vector2 dir = e->GetPosition() - owner->GetPosition();
        float dist = dir.Length();
        if (dist < AttackRange)
        {
            SDL_Log("Enemy in range");
            // Rotate to face enemy
            owner->SetRotation(Math::Atan2(-dir.y, dir.x));
            // Spawn bullet at tower position facing enemy
            Bullet* b = new Bullet(owner->GetGame());
            b->SetPosition(owner->GetPosition());
            b->SetRotation(owner->GetRotation());
            mNextAttack = AttackTime;
        }
        // If there's no enemy in range, transistion back to patrol
        else
        {
            mOwner->ChangeState("Patrol");
        }
    }
    // Else if there aren't any enemies at all, transition back to patrol
    else if (e == nullptr)
    {
        mOwner->ChangeState("Patrol");
    }
}

void AIAttack::OnEnter()
{
	SDL_Log("Entering %s state", GetName());
}

void AIAttack::OnExit()
{
	SDL_Log("Exiting %s state", GetName());
}
