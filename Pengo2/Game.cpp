#include "Game.h"

#include <InputManager.h>
#include <InputCommands.h>

#include <TransformComponent.h>
#include <TextureComponent.h>
#include <RenderComponent.h>
#include <BoxColliderComponent.h>
#include <ActorComponent.h>
#include "CollisionObserver.h"
#include "HitObserver.h"
#include <Helpers.h>
#include <FSM.h>



void Game::Initialize()
{
    auto& scene = GameEngine::SceneManager::GetInstance().CreateScene("Demo");

    m_pActor = std::make_unique<GameEngine::GameObject>();
    m_pActor->AddComponent<GameEngine::BoxCollider>(250, 350, 20, 20);
    m_pActor->AddComponent<GameEngine::TransformComponent>(glm::vec3(250, 350, 0));
    m_pActor->AddComponent<GameEngine::TextureComponent>("Pengo.tga"); 
    m_pActor->AddComponent<GameEngine::RenderComponent>();
    m_pActor->AddComponent<GameEngine::ActorComponent>();

    m_pBlackboard = new Blackboard(); 

    m_IdleState = new IdleState();
    m_RunningState = new RunningState(); 

    m_IsInputGiven = new GameEngine::IsInputGiven();
    m_IsNotInputGiven = new GameEngine::IsInputNotGiven(); 

    m_pActor->AddComponent<GameEngine::FSM>(m_IdleState, m_pBlackboard); 
    m_pActor->GetComponent<GameEngine::FSM>()->AddTransition(m_IdleState, m_RunningState, m_IsInputGiven);
    m_pActor->GetComponent<GameEngine::FSM>()->AddTransition(m_RunningState, m_IdleState, m_IsNotInputGiven);

    m_pActor->AddComponent<CollisionObserver>();
    m_pActor->AddComponent<HitObserver>();
    GameEngine::GameObject* referenceToCharacterPengo = m_pActor.get();
    auto collisionObserverComponent = m_pActor->GetComponent<CollisionObserver>();
    auto hitObserverComponent = m_pActor->GetComponent<HitObserver>(); 
    scene.Add(std::move(m_pActor));

    m_pEnvironment = std::make_unique<GameEngine::GameObject>();
    m_pEnvironment->AddComponent<Environment>("Level.json", &scene);
    m_pEnvironment->GetComponent<Environment>()->SetActor(referenceToCharacterPengo);
   
    m_pEnvironment->GetComponent<Environment>()->AttachObserver<GameEngine::CollisionState>(collisionObserverComponent);
    m_pEnvironment->GetComponent<Environment>()->AttachObserver<GameEngine::HitInfo>(hitObserverComponent);
   
    scene.Add(std::move(m_pEnvironment));

    InitializeInputSystem(referenceToCharacterPengo);
}

void Game::InitializeInputSystem(GameEngine::GameObject* gameActor)
{
    auto& input = GameEngine::InputManager::GetInstance(); 
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
}