// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner, float mass, int updateOrder)
:Component(owner, updateOrder)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
,mMass(mass)
,mSumForces(0.0f)
,mVelocity(0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
    // Code to determine rotation
	if (!Math::NearZero(mAngularSpeed))
	{
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}
    
    // Code to calculate distance based on the forces
    float accel = mSumForces / mMass;
    //float distance = (mVelocity*deltaTime) + (0.5f * accel * deltaTime * deltaTime);
    // Adjust the velocity based on acceleration
    mVelocity = mVelocity + (accel * deltaTime);
	
    // Code to determine forward movement
	if (!Math::NearZero(mVelocity))
	{
		Vector2 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mVelocity * deltaTime;
		
		// (Screen wrapping code only for asteroids)
		if (pos.x < 0.0f) { pos.x = 1022.0f; }
		else if (pos.x > 1024.0f) { pos.x = 2.0f; }

		if (pos.y < 0.0f) { pos.y = 766.0f; }
		else if (pos.y > 768.0f) { pos.y = 2.0f; }

		mOwner->SetPosition(pos);
	}
    
    // Reset mSumForces for the frame
    mSumForces = 0;
}

void MoveComponent::AddForce(Vector2 force)
{
    // Force is given as (magnitude, direction)
    // For now, assume that the direction is always forward (0 radians)
    // Note: I may come back and code direction later
    
    // Add the magnitude to the sum of forces
    mSumForces += force.x;
    
}
