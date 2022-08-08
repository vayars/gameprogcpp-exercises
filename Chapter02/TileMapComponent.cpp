//
//  TileMapComponent.cpp
//  Game-mac
//
//  Created by Veronica Ayars on 7/1/22.
//  Copyright © 2022 Sanjay Madhav. All rights reserved.
//

#include "TileMapComponent.h"
#include "Actor.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>

TileMapComponent::TileMapComponent(Actor* owner, int drawOrder)
    :SpriteComponent(owner, drawOrder)
    ,mTileMap(nullptr)
    ,mDrawOrder(drawOrder)
    ,mTileWidth(32)
    ,mTileHeight(32)
{
}
    
void TileMapComponent::Draw(SDL_Renderer* renderer)
{
    if (mTileMap)
    {
        SDL_Rect destR;
        SDL_Rect srcR;
        
        // For each element of the loaded tile map
        for (int y = 0; y < mTileMap->size(); y++)
        {
            for (int x = 0; x < mTileMap->at(0).size(); x++)
            {
                // Get tile number
                int tileNum = mTileMap->at(y).at(x);
                // Skip the tile if tileNum is -1
                if (tileNum == -1)
                {
                    continue;
                }
                // Scale the width/height by owner's scale
                destR.w = static_cast<int>(mTileWidth * mOwner->GetScale());
                destR.h = static_cast<int>(mTileHeight * mOwner->GetScale());
                // Draw the rectangle at the appropriate coordinates
                destR.x = static_cast<int>(x*32);
                destR.y = static_cast<int>(y*32); 
                // Scale the tile width/height
                srcR.w = static_cast<int>(mTileWidth * mOwner->GetScale());
                srcR.h = static_cast<int>(mTileHeight * mOwner->GetScale());
                // Calculate the tile's coordinates on the tile map
                srcR.x =  (tileNum % 8) * 32;
                srcR.y = (tileNum / 8) * 32;
                
                // Draw (have to convert angle from radians to degrees, and clockwise to counter)
                SDL_RenderCopyEx(renderer,
                    mTexture,
                    &srcR,
                    &destR,
                    -Math::ToDegrees(mOwner->GetRotation()),
                    nullptr,
                    SDL_FLIP_NONE);
            }
        }
    }
}

void TileMapComponent::LoadTileMap(std::string fileName)
{
    std::vector<std::vector<int>> content;
    mTileMap = new (std::vector<std::vector<int>>);
    std::vector<int> row;
    std::string line, word;
    
    // Open the CSV files and read in the tile numbers line by line
    std::fstream file (fileName, std::ios::in);
    if(file.is_open())
    {
        while(getline(file, line))
        {
            row.clear();
            std::stringstream str(line);
            
            while(getline(str, word, ','))
            {
                row.push_back(stoi(word));
            }
            mTileMap->push_back(row);
        }
    }
}
