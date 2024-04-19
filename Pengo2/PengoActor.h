#pragma once
#include <memory>
#include <GameObject.h>
#include <StatesAndTransitions.h>

class GameEngine::Blackboard;
class GameEngine::IdleState;
class GameEngine::RunningState;
class GameEngine::IsInputGiven;
class GameEngine::IsInputNotGiven;

class HitObserver;
class CollisionObserver;
class PushObserver;

class PengoActor final
{
public:
	PengoActor();
	~PengoActor(); 

	PengoActor(const PengoActor& other) = delete;
	PengoActor& operator=(const PengoActor& other) = delete;
	PengoActor(PengoActor&& other) noexcept = delete;
	PengoActor& operator=(PengoActor&& other) noexcept = delete;

	GameEngine::GameObject* GetReferenceToActor() const
	{
		return m_ReferenceToCharacterPengo;
	}

	HitObserver* GetHitObserver() const;
	PushObserver* GetPushObserver() const;
	std::unique_ptr<GameEngine::GameObject>& GetActorGameObject();

private:
	std::unique_ptr<GameEngine::GameObject> m_pActor;

	std::unique_ptr<GameEngine::Blackboard> m_pBlackboard;

	std::unique_ptr<GameEngine::IdleState> m_IdleState;
	std::unique_ptr<GameEngine::RunningState> m_RunningState;

	std::unique_ptr<GameEngine::IsInputGiven> m_IsInputGiven;
	std::unique_ptr<GameEngine::IsInputNotGiven> m_IsNotInputGiven;

	int m_HorizontalAmountOfFrames = 8;
	int m_VerticalAmountOfFrames = 3;

	GameEngine::GameObject* m_ReferenceToCharacterPengo;
};
