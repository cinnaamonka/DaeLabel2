#pragma once
#include <BaseComponent.h>
#include <IObserver.h>
#include <Helpers.h>

class GameObject;

class HitObserver final : public GameEngine::BaseComponent, public GameEngine::IObserver<GameEngine::HitInfo>
{
public:

	HitObserver(GameEngine::GameObject* pGameObject);
	~HitObserver() = default;
	HitObserver(const HitObserver& other) = delete;
	HitObserver& operator=(const HitObserver& other) = delete;
	HitObserver(HitObserver&& other) = delete;

	void Notify(const GameEngine::HitInfo& message_from_subject) override;

private:
	void ResetHorizontalPosition(const GameEngine::Rect& actorShape, const GameEngine::HitInfo& hitInfo);
	void ResetVerticalPosition(const GameEngine::Rect& actorShape, const GameEngine::HitInfo& hitInfo);
private:
	const float m_TopOffset = 0.9f;
	const float m_BottomOffset = 0.1f;

};

