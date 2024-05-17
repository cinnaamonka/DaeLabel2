#include "ScoreObserver.h"
#include <TransformComponent.h>
#include <TextureComponent.h>
#include <RenderComponent.h>
#include <GameObject.h>
#include <BoxColliderComponent.h>
#include <FSM.h>
#include <AnimationComponent.h>

ScoreObserver::ScoreObserver(GameEngine::Scene* scenePtr)
{
	m_ScenePtr = scenePtr;
}

void ScoreObserver::Notify(const Score& message_from_subject)
{
	auto gameObject = std::make_unique<GameEngine::GameObject>();

	switch (message_from_subject.type)
	{
	case ScoreType::EggFound:
		gameObject->AddComponent<GameEngine::TextureComponent>("Score.tga");
		break;
	case ScoreType::EnemyKilled:
		gameObject->AddComponent<GameEngine::TextureComponent>("Score.tga");
		break;
	default:
		break;
	}
	gameObject->AddComponent<GameEngine::AnimationComponent>();

	//initialize
	m_pScoreAppearing = std::make_unique<ScoreAppearingState>();
	m_pScoreWaiting   = std::make_unique<ScoreWaitingState>();
	m_pIsScoreWaiting = std::make_unique<IsScoreWaiting>(); 

	gameObject->AddComponent<GameEngine::FSM>(m_pScoreWaiting.get(),
		gameObject->GetComponent<GameEngine::AnimationComponent>());

	gameObject->GetComponent<GameEngine::FSM>()->AddTransition(m_pScoreWaiting.get(), m_pScoreAppearing.get(),
		m_pIsScoreWaiting.get());

	gameObject->AddComponent<GameEngine::TransformComponent>(message_from_subject.pos);

	auto textureSizeX = gameObject->GetComponent<GameEngine::TextureComponent>()->GetTexture()->GetSize().x / m_HorizontalAmountOfFrames;
	auto textureSizeY = gameObject->GetComponent<GameEngine::TextureComponent>()->GetTexture()->GetSize().y;

	gameObject->GetComponent<GameEngine::TransformComponent>()->SetDimensions({ 0, 0,textureSizeX,textureSizeY });

	gameObject->AddComponent<GameEngine::RenderComponent>();
	
	m_ScenePtr->Add(std::move(gameObject)); 
}