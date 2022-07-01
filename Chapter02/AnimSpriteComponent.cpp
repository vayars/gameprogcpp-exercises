// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AnimSpriteComponent.h"
#include "Math.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder)
	:SpriteComponent(owner, drawOrder)
	, mCurrFrame(0.0f)
	, mAnimFPS(5.0f)
{
}

void AnimSpriteComponent::Update(float deltaTime)
{
	SpriteComponent::Update(deltaTime);

	if (mAnimTextures.size() > 0)
	{
		// Update the current frame based on frame rate
		// and delta time
		mCurrFrame += mAnimFPS * deltaTime;
        
        // Check which animation is running
        // If it doesn't loop and it's past the range, transfer to the default anim
        if (!mIsLooping.at(mCurrAnim))
        {
            if (mCurrFrame > mAnimRanges.at(mCurrAnim).back()){
                mCurrAnim = mDefaultAnim;
                mCurrFrame = mAnimRanges.at(mCurrAnim).front();
            }
        }
        // If it does loop, wrap current frame
        else
        {
            // Wrap current frame if needed
            while (mCurrFrame > mAnimRanges.at(mCurrAnim).back())
            {
                mCurrFrame -= mAnimRanges.at(mCurrAnim).back();
            }
            // Wrap current frame to the beginning of the animation if needed
            if (mCurrFrame < mAnimRanges.at(mCurrAnim).front())
            {
                mCurrFrame = mAnimRanges.at(mCurrAnim).front();
            }
        }
		// Set the current texture
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
	}
}

void AnimSpriteComponent::SetAnimTextures(
                                          const std::vector<SDL_Texture*>& textures,
                                          const std::unordered_map<std::string, std::vector<int>>& animRanges,
                                          std::unordered_map<std::string, bool>& isLooping,
                                          std::string defaultAnim,
                                          std::string start)
{
	mAnimTextures = textures;
    mAnimRanges = animRanges;
    mIsLooping = isLooping;
    mDefaultAnim = defaultAnim;
    mCurrAnim = start;
    
	if (mAnimTextures.size() > 0)
	{
		// Set the active texture to first frame of the start animation
		mCurrFrame = mAnimRanges.at(start).at(0);
		SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
	}
}
