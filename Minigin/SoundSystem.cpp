#include "SoundSystem.h"

#include <vector>
#include <thread>
#include <iostream>
#include <mutex>
#include <future>
#include "Helpers.h"
#include <unordered_map>
#include "RingBuffer.h"

#include <SDL.h>
#include <SDL_mixer.h>

namespace GameEngine
{
	class SoundSystem::SoundSystemImpl
	{
	public:
		SoundSystemImpl() :
			m_SoundQueue{},
			m_pSoundEffects{},
			m_ActiveAudio{ static_cast<size_t>(MIX_CHANNELS), nullptr }
		{};
		void Load(const std::string& filePath, sound_id id)
		{
			Mix_Chunk* soundEffect = Mix_LoadWAV(filePath.c_str());
			if (soundEffect == nullptr)
			{
				std::cerr << "Couldn't load sound: " << Mix_GetError() << std::endl;
				return;
			}

			std::lock_guard<std::mutex> lock(m_SoundEffectsMutex);

			m_pSoundEffects.insert({ id, std::shared_ptr<Mix_Chunk>(soundEffect, Mix_FreeChunk) });
		}

		void Update()
		{
			{
				std::lock_guard<std::mutex> lock(m_SoundEffectsMutex);

				if (m_SoundQueue.GetPending() == 0)
					return;
			}

			if (m_AsyncResult.valid())
			{
				std::cout << "Waiting for previous task to complete..." << std::endl;

				// not necessary to call get() here because of launch policy
				// , but before program ends makes the behavior clearer and can throw exceptions
				m_AsyncResult.get();
				std::cout << "Previous task completed." << std::endl;

			}
			std::cout << "Launching new task..." << std::endl;

			m_AsyncResult = std::async(std::launch::async, [this] { AsyncUpdate(); }); 

			std::cout << "New task launched." << std::endl;


		}
		void Play(const sound_id id, const float volume)
		{
			m_SoundQueue.PushBack({ id, volume });
		}
		bool Contains(const sound_id id) const
		{
			if(m_pSoundEffects.find(id) != m_pSoundEffects.end()) return true;
			return false;
		}

		bool IsPlaying(const sound_id id) const
		{
			std::shared_ptr<Mix_Chunk> pChunk = nullptr; 

			if (id != UINT_MAX)
			{
				auto iter = std::find_if(m_pSoundEffects.begin(), m_pSoundEffects.end(),
					[id](const auto& pair) { return pair.first == id; });

				if (iter != m_pSoundEffects.end())
					pChunk = iter->second;
			}

			for (int i = 0; i < MIX_CHANNELS; ++i)
			{
				if (Mix_Playing(i) && Mix_GetChunk(i) == pChunk.get())return true;
			}
			return false;
		}

		void Pause()
		{
			Mix_Pause(-1);
		}

		void Resume()
		{
			Mix_Resume(-1);
		}

		void Stop()
		{
			Mix_HaltChannel(-1);
		}

		void CleanUp()
		{
			for (auto pSound : m_pSoundEffects)
			{
				Mix_FreeChunk(pSound.second.get());
			}

			Mix_CloseAudio();
			Mix_Quit();
			SDL_Quit();
		}
		~SoundSystemImpl() = default;
	private:
		void AsyncUpdate()
		{
			std::cout << "Task started." << std::endl;
			std::lock_guard<std::mutex> lock(m_SoundEffectsMutex);

			Sound sound = m_SoundQueue.GetFront();
			std::shared_ptr<Mix_Chunk> pChunk = nullptr;

			if (sound.id != UINT_MAX)
			{
				auto iter = std::find_if(m_pSoundEffects.begin(), m_pSoundEffects.end(),
					[&sound](auto& pair) { return pair.first == sound.id; });

				if (iter != m_pSoundEffects.end())
					pChunk = iter->second;
			}
			if (!pChunk)
			{
				std::cerr << "Failed to load audio chunk for sound ID: " << sound.id << std::endl;
				return;
			}

			pChunk->volume = static_cast<Uint8>(sound.volume);

			int channel = Mix_PlayChannel(-1, pChunk.get(), 0);
			if (channel == -1)
				return;

			// Keep shared pointer alive while sound is playing
			m_ActiveAudio[channel] = pChunk;

			m_SoundQueue.PopFront();
			std::cout << "Task completed." << std::endl;
		}
	private:

		// use shared ptr because the sounds should finish playing before being destroyed
		RingBuffer<Sound, 16> m_SoundQueue;
		std::unordered_map<sound_id, std::shared_ptr<Mix_Chunk>> m_pSoundEffects;
		std::vector<std::shared_ptr<Mix_Chunk>> m_ActiveAudio;
		std::mutex m_SoundEffectsMutex;
		std::future<void> m_AsyncResult;
	};

	SoundSystem::SoundSystem() :
		m_pImpl(nullptr)
	{
		m_pImpl = std::make_unique<SoundSystemImpl>();

		if (Mix_Init(SDL_INIT_AUDIO) < 0)
		{
			std::cerr << "Failed to initialize Sound System: " << SDL_GetError() << std::endl;
			return;
		}

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
		{
			std::cerr << "Failed to initialize SDL_mixer: " << Mix_GetError() << std::endl;
			return;
		}
	}

	SoundSystem::~SoundSystem()
	{
		m_pImpl->CleanUp();
	}

	void SoundSystem::Play(const sound_id id, const float volume)
	{
		if (!IsPlaying(id))
		{
			m_pImpl->Play(id, volume);
		}
	}

	void SoundSystem::Pause()
	{
		m_pImpl->Pause();
	}

	void SoundSystem::Resume()
	{
		m_pImpl->Resume();
	}

	void SoundSystem::Stop()
	{
		m_pImpl->Stop();
	}

	void SoundSystem::Load(const std::string& filePath, const sound_id id)
	{
		m_pImpl->Load(filePath, id);
	}
	void SoundSystem::Update()
	{
		m_pImpl->Update();
	}

	bool SoundSystem::Contains(const sound_id id) const
	{
		return m_pImpl->Contains(id);
	}

	bool SoundSystem::IsPlaying(const sound_id id) const
	{
		return m_pImpl->IsPlaying(id);
	}


}