#pragma once
#include <memory>
#include <GameObject.h>
#include <Helpers.h>
#include <BoxColliderComponent.h>
#include <Scene.h>

#include "HitObserver.h"

class BoxCollider;
class HitObserver;

class BaseBlock
{
public:
	BaseBlock(const glm::vec3& position, GameEngine::Scene* scene,const std::string& filename); 
	~BaseBlock() = default;

	BaseBlock(const BaseBlock& other) = delete;
	BaseBlock& operator=(const BaseBlock& other) = delete;
	BaseBlock(BaseBlock&& other) noexcept = delete;
	BaseBlock& operator=(BaseBlock&& other) noexcept = delete;

	bool IsCollidingHorizontally(const GameEngine::Rect& rectShape, GameEngine::HitInfo& hitInfo);
	bool IsCollidingVertically(const GameEngine::Rect& rectShape, GameEngine::HitInfo& hitInfo); 

	void PushBlock(const glm::vec3& direction);

	const glm::vec3& GetPosition()
	{
		return m_Position;
	}

	GameEngine::Rect& GetBlockShape()
	{
		return m_pBoxCollider->GetBoxCollider();
	}

	HitObserver* GetHitObserver()
	{
		auto observer = m_pGameObjectReference->GetComponent<HitObserver>();
		if(observer)
		return observer;
	}

	void SetPushSpeed(float speed)
	{
		m_PushSpeed = speed;
	}

	const float GetPushSpeed() const
	{
		return m_PushSpeed;
	}

private:

	std::unique_ptr<GameEngine::GameObject> m_pGameObject;
	GameEngine::BoxCollider* m_pBoxCollider;
	GameEngine::GameObject* m_pGameObjectReference;

	const int m_BlockSize;
	glm::vec3 m_Position;
	float m_PushSpeed;

};

