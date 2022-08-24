//
//  FireActor.cpp
//  Game-mac
//
//  Created by Veronica Ayars on 8/22/22.
//  Copyright © 2022 Sanjay Madhav. All rights reserved.
//

#include "FireActor.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "MeshComponent.h"
#include <SDL/SDL_log.h>

FireActor::FireActor(Game* game)
    :Actor(game)
{
    mChangeDirection = 5.0f;
    
    mMoveComp = new MoveComponent(this);
    mMoveComp->SetForwardSpeed(200.0f);
    mAudioComp = new AudioComponent(this);
    mAudioComp->PlayEvent("event:/FireLoop");
}

void FireActor::UpdateActor(float deltaTime)
{
    Actor::UpdateActor(deltaTime);
    
    // If the actor has moved for 5 seconds, switch directions
    if (mChangeDirection <= 0.0f)
    {
        mMoveComp->SetForwardSpeed(-mMoveComp->GetForwardSpeed());
        mChangeDirection = 5.0f;
    }
    mChangeDirection -= deltaTime;
    //SDL_Log("Sphere Pos: %f, %f, %f", GetPosition().x, GetPosition().y, GetPosition().z);
    //SDL_Log("Sphere Virtual Pos: %f, %f, %f", mAudioComp->GetVirtualPos().x, mAudioComp->GetVirtualPos().y, mAudioComp->GetVirtualPos().z);
}
