// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AudioComponent.h"
#include "Actor.h"
#include "Game.h"
#include "AudioSystem.h"

AudioComponent::AudioComponent(Actor* owner, int updateOrder)
	:Component(owner, updateOrder)
{
}

AudioComponent::~AudioComponent()
{
	StopAllEvents();
}

void AudioComponent::Update(float deltaTime)
{
	Component::Update(deltaTime);

	// Remove invalid 2D events
	auto iter = mEvents2D.begin();
	while (iter != mEvents2D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents2D.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// Remove invalid 3D events
	iter = mEvents3D.begin();
	while (iter != mEvents3D.end())
	{
		if (!iter->IsValid())
		{
			iter = mEvents3D.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void AudioComponent::OnUpdateWorldTransform()
{
	// Update 3D events' world transforms
	//Matrix4 world = mOwner->GetWorldTransform();
    // Replace ^^ with the Audio Comp Virtual World member variable
    ComputeVirtualWorldTransform();
	for (auto& event : mEvents3D)
	{
		if (event.IsValid())
		{
			event.Set3DAttributes(mVirtualWorldTransform, mLastVirtualWorldTransform);
		}
	}
}

Matrix4 AudioComponent::ComputeVirtualWorldTransform()
{
    // Set the current virtual world transform to the last one
    mLastVirtualWorldTransform = mVirtualWorldTransform;
    // Calculate the virtual position
    mVirtualPosition = mOwner->GetGame()->ComputeVirtualPos(mOwner->GetPosition(), mVirtualPosition);
    // Calculate the new virtual world transform
    mVirtualWorldTransform = Matrix4::CreateScale(mOwner->GetScale());
    mVirtualWorldTransform *= Matrix4::CreateFromQuaternion(mOwner->GetRotation());
    mVirtualWorldTransform *= Matrix4::CreateTranslation(mVirtualPosition);
    
    return mVirtualWorldTransform;
}

SoundEvent AudioComponent::PlayEvent(const std::string& name)
{
	SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);
	// Is this 2D or 3D?
	if (e.Is3D())
	{
		mEvents3D.emplace_back(e);
		// Set initial 3D attributes
        // Replace with virtual world and last virtual world
		e.Set3DAttributes(mVirtualWorldTransform,
                          mLastVirtualWorldTransform);
	}
	else
	{
		mEvents2D.emplace_back(e);
	}
	return e;
}

void AudioComponent::StopAllEvents()
{
	// Stop all sounds
	for (auto& e : mEvents2D)
	{
		e.Stop();
	}
	for (auto& e : mEvents3D)
	{
		e.Stop();
	}
	// Clear events
	mEvents2D.clear();
	mEvents3D.clear();
}
