//
//  TileMapComponent.h
//  Game-mac
//
//  Created by Veronica Ayars on 7/1/22.
//  Copyright © 2022 Sanjay Madhav. All rights reserved.
//

#pragma once
#include "SpriteComponent.h"
#include <vector>
#include <string>

class TileMapComponent : public SpriteComponent
{
public:
    TileMapComponent(class Actor* owner, int drawOrder);
    
    void Draw(SDL_Renderer* renderer) override;
    
    void LoadTileMap(std::string fileName);
protected:
    // A 2D vector detailing which tiles to draw
    std::vector<std::vector<int>>* mTileMap;
    int mDrawOrder;
    int mTileWidth;
    int mTileHeight;
};
