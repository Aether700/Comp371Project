#include "SoundManager.h"

SoundManager* SoundManager::s_instance;

void SoundManager::Init()
{
	if (s_instance != nullptr)
	{
		assert(false);
	}

	s_instance = new SoundManager();
}

void SoundManager::Play(const std::string& filepath, bool loop)
{
	s_instance->m_soundEngine->play2D(filepath.c_str(), loop);
}

void SoundManager::Shutdown()
{
	delete s_instance;
}

SoundManager::SoundManager() : m_soundEngine(irrklang::createIrrKlangDevice())
{
	if (m_soundEngine == nullptr)
	{
		std::cout << "Could not initialize the Sound Engine\n";
		assert(false);
	}
}

SoundManager::~SoundManager()
{
	//delete the sound engine by calling drop on it
	m_soundEngine->drop();
}

