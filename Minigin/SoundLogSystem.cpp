#include "SoundLogSystem.h"
#include <iostream>

GameEngine::SoundLogSystem::SoundLogSystem(std::unique_ptr<BaseSoundSystem>&& soundSystem)
{
	pSoundSystem = std::move(soundSystem);
	std::cout << "Sound log system created " << std::endl;
}

void GameEngine::SoundLogSystem::Play(const unsigned short id, const float volume)
{
	std::cout << "Playing sound with id: " << id << " and volume: " << volume << std::endl;
	pSoundSystem->Play(id, volume);
}

void GameEngine::SoundLogSystem::Pause()
{
	std::cout << "Pausing sound" << std::endl;
	pSoundSystem->Pause();
}

void GameEngine::SoundLogSystem::Resume()
{
	std::cout << "Resuming sound" << std::endl;
	pSoundSystem->Resume();
}

void GameEngine::SoundLogSystem::Stop()
{
	std::cout << "Stopping sound" << std::endl;
	pSoundSystem->Stop();
}

void GameEngine::SoundLogSystem::Load(const std::string& filePath, const sound_id id)
{
	std::cout << "Loading sound from file: " << filePath << std::endl;
	pSoundSystem->Load(filePath, id);
}

void GameEngine::SoundLogSystem::Update()
{
	pSoundSystem->Update();
}

