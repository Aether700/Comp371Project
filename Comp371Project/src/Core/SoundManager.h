#pragma once
#include "../Dependencies/irrKlang-64bit-1.6.0/include/irrKlang.h"

#include <iostream>
#include <assert.h>

/* Singleton class responsible of playing sounds
*/
class SoundManager
{
public:
	static void Init();

	static void Play(const std::string& filepath, bool loop = false);

	static void Shutdown();

private:
	SoundManager();

	~SoundManager();

	static SoundManager* s_instance;

	irrklang::ISoundEngine* m_soundEngine;
};