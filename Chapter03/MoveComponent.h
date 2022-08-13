// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
public:
	// Lower update order to update first
	MoveComponent(class Actor* owner, float mass = 1.0f, int updateOrder = 10);

	void Update(float deltaTime) override;
	
	float GetAngularSpeed() const { return mAngularSpeed; }
	float GetForwardSpeed() const { return mForwardSpeed; }
    float GetVelocity() const { return mVelocity; }
	void SetAngularSpeed(float speed) { mAngularSpeed = speed; }
	void SetForwardSpeed(float speed) { mForwardSpeed = speed; }
    void SetVelocity(float velocity) { mVelocity = velocity; }
    
    void AddForce(Vector2 force);
private:
	// Controls rotation (radians/second)
	float mAngularSpeed;
	// Controls forward movement (units/second)
	float mForwardSpeed;
    // The mass of the object being moved
    float mMass;
    // The sum of the forces acting on the object
    float mSumForces;
    // The velocity of the object
    float mVelocity;
};
