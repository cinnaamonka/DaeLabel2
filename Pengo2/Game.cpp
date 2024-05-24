#include "Game.h"

#include <InputManager.h>
#include <ResourceManager.h>
#include <InputCommands.h>

#include "HitObserver.h"
#include "EggObserver.h"

#include "EnvironmentObserver.h"
#include "BlockComponent.h"
#include "TextComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "PengoInputCommands.h"
#include <ActorComponent.h>


#include <SoundSystem.h>
#include <SoundServiceLocator.h>
#include <SoundLogSystem.h>
#include <TimeManager.h>


#include <HUD.h>
#include <thread>
#include "Structs.h"

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <SDL_mixer.h>

//#include <vld.h>
bool Game::m_IsLevelComplete = false;

void Game::Initialize(int levelIndex)
{
	const std::string levelName = "Level" + std::to_string(levelIndex) + ".json";

	GameEngine::LevelInfo levelInfo{};

	// Load level blocks
	GameEngine::GetLevelInfo(levelName, levelInfo);

	auto& scene = GameEngine::SceneManager::GetInstance().CreateScene(levelName);

	m_pPengoActor = std::make_unique<PengoActor>();
	m_pEggsObserver = std::make_unique<EggObserver>(&scene);
	m_pScoreObserver = std::make_unique<ScoreObserver>(&scene);

	auto hitObserverComponent = m_pPengoActor->GetHitObserver();
	m_pPengoActor->GetActorGameObject()->GetComponent<GameEngine::ActorComponent>()->AttachObserver(this);

	m_pEnvironment = std::make_unique<GameEngine::GameObject>();
	m_pEnvironment->AddComponent<Environment>(levelInfo.levelBlocks, &scene);
	m_pEnvironment->AddComponent<EnvironmentObserver>();
	m_pEnvironment->GetComponent<Environment>()->SetActor(m_pPengoActor->GetReferenceToActor());

	m_pEnemyManager = std::make_unique<EnemyManager>(static_cast<int>(levelInfo.enemiesPositions.size()), levelInfo.enemiesPositions, &scene,
		m_pPengoActor->GetReferenceToActor());

	m_pEnvironment->GetComponent<Environment>()->SetEnemyManager(m_pEnemyManager.get());
	m_pEnvironment->GetComponent<Environment>()->AttachObserver<GameEngine::HitInfo>(hitObserverComponent);
	m_pEnvironment->GetComponent<Environment>()->AttachObserver<glm::vec3>(m_pEggsObserver.get());
	m_pEnvironment->GetComponent<Environment>()->AttachObserver<EventInfo>(m_pEnvironment->GetComponent<EnvironmentObserver>());
	m_pEnvironment->GetComponent<Environment>()->AttachObserver<Score>(m_pScoreObserver.get());

	m_pEnvironmentReference = m_pEnvironment.get();

	scene.Add(std::move(m_pEnvironment));
	scene.Add(std::move(m_pPengoActor->GetActorGameObject()));

	InitializeInputSystem(m_pPengoActor->GetReferenceToActor());

	//initialize HUD
	m_pHUD = std::make_unique<GameEngine::HUD>();
	m_pHUD->AddScoreBar(levelInfo.hudPositions["ScoreBar"], &scene);
	m_pHUD->AddLifeBar(levelInfo.hudPositions["LifeBar"], &scene, levelInfo.lifesAmount);
	m_pHUD->CreateGameMode(levelInfo.hudPositions["GameMode"], &scene, levelInfo.gameMode);
	m_pHUD->CreateSnoBeesBar(levelInfo.hudPositions["SnoBeesBar"], 3, &scene);

	m_pEnvironmentReference->GetComponent<Environment>()->AttachObserver<GameEngine::HUDEvent>(m_pHUD.get());

	//Sounds start one after another
	GameEngine::SoundServiceLocator::GetInstance().GetSoundSystemInstance().Play(static_cast<int>(PengoSounds::ActStarts), 20);

	Mix_ChannelFinished([](int)
		{
			GameEngine::SoundServiceLocator::GetInstance().GetSoundSystemInstance().Play(static_cast<int>(PengoSounds::Background), 20);
			Mix_ChannelFinished(nullptr);
		});

	while (GameEngine::SoundServiceLocator::GetInstance().GetSoundSystemInstance().IsPlaying(static_cast<int>(PengoSounds::ActStarts)))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}


}

bool Game::IsLevelComplete()
{
	return m_IsLevelComplete;
}

void Game::Notify(const GameEngine::State& messageFromSubject)
{
	switch (messageFromSubject)
	{
	case GameEngine::State::PlayerDied:
		GameEngine::SoundServiceLocator::GetInstance().GetSoundSystemInstance().Play(static_cast<int>(PengoSounds::PlayerDeath), 20);


		Mix_ChannelFinished([](int)
			{
				m_IsLevelComplete = true;
				Mix_ChannelFinished(nullptr);
			});

		while (GameEngine::SoundServiceLocator::GetInstance().GetSoundSystemInstance().IsPlaying(static_cast<int>(PengoSounds::ActStarts)))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		break;
	case GameEngine::State::Victory:
		GameEngine::SoundServiceLocator::GetInstance().GetSoundSystemInstance().Stop(static_cast<int>(PengoSounds::Background));

		m_IsLevelComplete = true;
		break;
	}

}

void Game::InitializeInputSystem(GameEngine::GameObject* gameActor)
{
	auto& input = GameEngine::InputManager::GetInstance();

	input.CleanUp();

	auto m_Controller = std::make_unique<GameEngine::Controller>(0);
	auto m_Keyboard = std::make_unique<GameEngine::Keyboard>();
	input.AddDevice(std::move(m_Controller));
	input.AddDevice(std::move(m_Keyboard));

	input.AddCommand<GameEngine::Controller>(
		GameEngine::InputControllerBinding{ GameEngine::DeviceButton::XINPUT_GAMEPAD_DRAD_LEFT, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ -1, 0, 0 }));

	input.AddCommand<GameEngine::Controller>(
		GameEngine::InputControllerBinding{ GameEngine::DeviceButton::XINPUT_GAMEPAD_DRAD_RIGHT, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ 1, 0, 0 }));

	input.AddCommand<GameEngine::Controller>(
		GameEngine::InputControllerBinding{ GameEngine::DeviceButton::XINPUT_GAMEPAD_DRAD_UP, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ 0, -1, 0 }));

	input.AddCommand<GameEngine::Controller>(
		GameEngine::InputControllerBinding{ GameEngine::DeviceButton::XINPUT_GAMEPAD_DRAD_DOWN, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ 0, 1, 0 }));

	input.AddCommand<GameEngine::Controller>(
		GameEngine::InputControllerBinding{ GameEngine::DeviceButton::XINPUT_CONTROLLER_A, GameEngine::InputState::Previous },
		std::make_unique<PushBlockCommand>(m_pEnvironmentReference));

	//Keyboard Input
	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_A, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ -1,0, 0 }));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_D, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ 1,0, 0 }));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_W, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ 0,-1, 0 }));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_S, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::MoveCommand>(gameActor, glm::vec3{ 0,1, 0 }));

	//Keyboard Input
	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_A, GameEngine::InputState::Released },
		std::make_unique<GameEngine::StopCollisionCheck>(gameActor));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_D, GameEngine::InputState::Released },
		std::make_unique<GameEngine::StopCollisionCheck>(gameActor));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_W, GameEngine::InputState::Released },
		std::make_unique<GameEngine::StopCollisionCheck>(gameActor));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_S, GameEngine::InputState::Released },
		std::make_unique<GameEngine::StopCollisionCheck>(gameActor));



	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_SPACE, GameEngine::InputState::Previous },
		std::make_unique<PushBlockCommand>(m_pEnvironmentReference));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_SPACE, GameEngine::InputState::Released },
		std::make_unique<StopPushCommand>(m_pEnvironmentReference));

	input.AddCommand<GameEngine::Keyboard>(
		GameEngine::InputKeyboardBinding{ SDL_SCANCODE_SPACE, GameEngine::InputState::Previous },
		std::make_unique<GameEngine::PushCommand>(gameActor));
}

void Game::LoadSounds()
{
	auto& soundSystem = GameEngine::SoundServiceLocator::GetSoundSystemInstance();

	for (const auto& [soundType, filePath] : SOUND_PATH_MAPPING)
	{
		soundSystem.Load(filePath, static_cast<GameEngine::sound_id>(static_cast<int>(soundType)));
	}
}

