//
//  FireActor.h
//  Game-mac
//
//  Created by Veronica Ayars on 8/22/22.
//  Copyright © 2022 Sanjay Madhav. All rights reserved.
//

#pragma once
#include "Actor.h"
#include "MoveComponent.h"

class FireActor : public Actor
{
public:
    FireActor(class Game* game);

    void UpdateActor(float deltaTime) override;
    
private:
    MoveComponent* mMoveComp;
    float mChangeDirection;
};
