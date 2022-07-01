// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SpriteComponent.h"
#include <vector>
#include <unordered_map>
#include <string>

class AnimSpriteComponent : public SpriteComponent
{
public:
	AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
	// Update animation every frame (overridden from component)
	void Update(float deltaTime) override;
	// Set the textures used for animation
	void SetAnimTextures(const std::vector<SDL_Texture*>& textures,
                         const std::unordered_map<std::string, std::vector<int>>& animRanges,
                         std::unordered_map<std::string, bool>& isLooping,
                         std::string defaultAnim,
                         std::string start);
	// Set/get the animation FPS
	float GetAnimFPS() const { return mAnimFPS; }
	void SetAnimFPS(float fps) { mAnimFPS = fps; }
private:
	// All textures in the animation
	std::vector<SDL_Texture*> mAnimTextures;
	// Current frame displayed
	float mCurrFrame;
	// Animation frame rate
	float mAnimFPS;
    // Range of textures for different animations
    std::unordered_map<std::string, std::vector<int>> mAnimRanges;
    // Current animation
    std::string mCurrAnim;
    // Default animation after completing non-looping animations
    std::string mDefaultAnim;
    // Check if the animation needs to loop
    std::unordered_map<std::string, bool> mIsLooping;
};
