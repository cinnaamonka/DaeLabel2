#pragma once
#include <GameObjectCommand.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>

class GameEngine::GameObject;

class PushBlockCommand :public GameEngine::GameObjectCommand
{
public:

public:
	explicit PushBlockCommand(GameEngine::GameObject* gameObject);
	~PushBlockCommand() = default;

	PushBlockCommand(const PushBlockCommand& other) = default;
	PushBlockCommand& operator=(const PushBlockCommand& other) = default;
	PushBlockCommand(PushBlockCommand&& other) noexcept = default;
	PushBlockCommand& operator=(PushBlockCommand&& other) noexcept = default;


	void Execute() override;
};
class StopPushCommand :public GameEngine::GameObjectCommand 
{
public:

public:
	explicit StopPushCommand(GameEngine::GameObject* gameObject);
	~StopPushCommand() = default;

	StopPushCommand(const StopPushCommand& other) = default;
	StopPushCommand& operator=(const StopPushCommand& other) = default;
	StopPushCommand(StopPushCommand&& other) noexcept = default;
	StopPushCommand& operator=(StopPushCommand&& other) noexcept = default;


	void Execute() override;
};

