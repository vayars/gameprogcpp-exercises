// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "InputSystem.h"
#include <SDL/SDL.h>
#include <cstring>

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrState[keyCode] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool MouseState::GetButtonValue(int button) const
{
	return (SDL_BUTTON(button) & mCurrButtons) == 1;
}

ButtonState MouseState::GetButtonState(int button) const
{
	int mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mCurrButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mCurrButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

// Empty costructor for vector<ControllerState>
ControllerState::ControllerState()
{
}

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
{
	return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
{
	if (mPrevButtons[button] == 0)
	{
		if (mCurrButtons[button] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrButtons[button] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool InputSystem::Initialize()
{
	// Keyboard
	// Assign current state pointer
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// Clear previous state memory
	memset(mState.Keyboard.mPrevState, 0,
		SDL_NUM_SCANCODES);

	// Mouse (just set everything to 0)
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mPrevButtons = 0;

	// Get the connected controller, if it exists
	//mController = SDL_GameControllerOpen(0);
	// Initialize controller state
	/*
    mState.Controller.mIsConnected = (mController != nullptr);
	memset(mState.Controller.mCurrButtons, 0,
		SDL_CONTROLLER_BUTTON_MAX);
	memset(mState.Controller.mPrevButtons, 0,
		SDL_CONTROLLER_BUTTON_MAX);
     */
    
    mNumControllers = 0;
    // Get a max of 4 open controllers
     for (int i = 0; i < SDL_NumJoysticks(); ++i)
    {
        // Test if the joystick is a controller
        if (SDL_IsGameController(i))
        {
            if (mNumControllers < 3)
            {
                // Open the conbtroller for use
                SDL_GameController* controller = SDL_GameControllerOpen(i);
                // Add to the vector of controllers and the controller states
                mControllers.emplace_back(controller);
                mState.Controllers.emplace_back();
                // Initialize the controller
                mState.Controllers.at(mNumControllers).mIsConnected =
                    (controller != nullptr);
                memset(mState.Controllers.at(mNumControllers).mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
                memset(mState.Controllers.at(mNumControllers).mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
                mNumControllers += 1;
                //SDL_Log("Controller number %d initialized.", mNumControllers);
            }
        }
    }
	return true;
}

void InputSystem::Shutdown()
{
}

void InputSystem::PrepareForUpdate()
{
	// Copy current state to previous
	// Keyboard
	memcpy(mState.Keyboard.mPrevState,
		mState.Keyboard.mCurrState,
		SDL_NUM_SCANCODES);

	// Mouse
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	mState.Mouse.mIsRelative = false;
	mState.Mouse.mScrollWheel = Vector2::Zero;

	// Controllers
	for (auto controller : mState.Controllers)
    {
        memcpy(controller.mPrevButtons,
            controller.mCurrButtons,
            SDL_CONTROLLER_BUTTON_MAX);
    }
}

void InputSystem::Update()
{
	// Mouse
	int x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		mState.Mouse.mCurrButtons = 
			SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mState.Mouse.mCurrButtons = 
			SDL_GetMouseState(&x, &y);
	}

	mState.Mouse.mMousePos.x = static_cast<float>(x);
	mState.Mouse.mMousePos.y = static_cast<float>(y);

	// Controller
    for (int index = 0; index < mNumControllers; index++)
    {
        // Buttons
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
        {
            mState.Controllers.at(index).mCurrButtons[i] =
                SDL_GameControllerGetButton(mControllers.at(index),
                    SDL_GameControllerButton(i));
        }

        // Triggers
        mState.Controllers.at(index).mLeftTrigger =
            Filter1D(SDL_GameControllerGetAxis(mControllers.at(index),
                SDL_CONTROLLER_AXIS_TRIGGERLEFT));
        mState.Controllers.at(index).mRightTrigger =
            Filter1D(SDL_GameControllerGetAxis(mControllers.at(index),
                SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

        // Sticks
        x = SDL_GameControllerGetAxis(mControllers.at(index),
            SDL_CONTROLLER_AXIS_LEFTX);
        y = -SDL_GameControllerGetAxis(mControllers.at(index),
            SDL_CONTROLLER_AXIS_LEFTY);
        mState.Controllers.at(index).mLeftStick = Filter2D(x, y);

        x = SDL_GameControllerGetAxis(mControllers.at(index),
            SDL_CONTROLLER_AXIS_RIGHTX);
        y = -SDL_GameControllerGetAxis(mControllers.at(index),
            SDL_CONTROLLER_AXIS_RIGHTY);
        mState.Controllers.at(index).mRightStick = Filter2D(x, y);
    }
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
	switch (event.type)
	{
        case SDL_MOUSEWHEEL:
            mState.Mouse.mScrollWheel = Vector2(
                static_cast<float>(event.wheel.x),
                static_cast<float>(event.wheel.y));
            break;
        case SDL_CONTROLLERDEVICEADDED:
        {
            //SDL_Log("Controller detected.");
            if (mNumControllers < 3)
            {
                // Open the conbtroller for use
                SDL_GameController* controller = SDL_GameControllerOpen(event.cdevice.which);
                // Check if the controller already exists
                auto iter = std::find(mControllers.begin(), mControllers.end(), controller);
                if (iter != mControllers.end())
                {
                    // If it already exists, close the duplicate
                    SDL_GameControllerClose(controller);
                    //SDL_Log("Duplicate controller detected; no controller added.");
                }
                else
                {
                    // Add to the vector of controllers and the controller states
                    mControllers.emplace_back(controller);
                    mState.Controllers.emplace_back();
                    // Initialize the controller
                    mState.Controllers.at(mNumControllers).mIsConnected =
                        (controller != nullptr);
                    memset(mState.Controllers.at(mNumControllers).mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
                    memset(mState.Controllers.at(mNumControllers).mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
                    mNumControllers += 1;
                    //SDL_Log("Controller successfully added at index %d.", mNumControllers);
                }
            }
            break;
        }
        case SDL_CONTROLLERDEVICEREMOVED:
        {
            //SDL_Log("Controller lost.");
            Sint32 id = event.cdevice.which;
            for (int i = 0; i < mNumControllers; i++)
            {
                // Get the instance ID of the game controller
                SDL_Joystick* joystick = SDL_GameControllerGetJoystick(mControllers.at(i));
                SDL_JoystickID joyID = SDL_JoystickInstanceID(joystick);
                // If the ID matches the one that was removed,
                if (id == joyID)
                {
                    // Close the controller
                    SDL_GameControllerClose(mControllers.at(i));
                    // Remove the SDL_GameController* from mControllers
                    mControllers.erase(mControllers.begin() + i);
                    // Remove the ControllerState from mState.Controllers
                    mState.Controllers.erase(mState.Controllers.begin() + i);
                    mNumControllers -= 1;
                    //SDL_Log("Controller successfully removed.");
                    break;
                }
            }
            break;
        }
        default:
            break;
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);

	mState.Mouse.mIsRelative = value;
}

float InputSystem::Filter1D(int input)
{
	// A value < dead zone is interpreted as 0%
	const int deadZone = 250;
	// A value > max value is interpreted as 100%
	const int maxValue = 30000;

	float retVal = 0.0f;

	// Take absolute value of input
	int absValue = input > 0 ? input : -input;
	// Ignore input within dead zone
	if (absValue > deadZone)
	{
		// Compute fractional value between dead zone and max value
		retVal = static_cast<float>(absValue - deadZone) /
		(maxValue - deadZone);
		// Make sure sign matches original value
		retVal = input > 0 ? retVal : -1.0f * retVal;
		// Clamp between -1.0f and 1.0f
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}

	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// Make into 2D vector
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// If length < deadZone, should be no input
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// Calculate fractional value between
		// dead zone and max value circles
		float f = (length - deadZone) / (maxValue - deadZone);
		// Clamp f between 0.0f and 1.0f
		f = Math::Clamp(f, 0.0f, 1.0f);
		// Normalize the vector, and then scale it to the
		// fractional value
		dir *= f / length;
	}

	return dir;
}
