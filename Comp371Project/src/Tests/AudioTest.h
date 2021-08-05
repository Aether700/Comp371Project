#pragma once
#include "../Core/SoundManager.h"
#include "../Core/Script.h"
#include "../Core/Input.h"
#include "../Core/Time.h"

#include <assert.h>
#include <iostream>

class AudioTest : public Script
{
public:
	void OnStart()
	{
		SoundManager::Play("Resources/Audio/breakout.mp3", true);
	}
};