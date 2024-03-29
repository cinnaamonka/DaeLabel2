#pragma once

#include <functional>
#include <string>

//#include <steam_api.h>

namespace GameEngine
{
	class Engine
	{
	public:
		explicit Engine(const std::string& dataPath);
		~Engine();
		void Run(const std::function<void()>& load);

		Engine(const Engine& other) = delete;
		Engine(Engine&& other) = delete;
		Engine& operator=(const Engine& other) = delete;
		Engine& operator=(Engine&& other) = delete;
	private:
		double m_Lag = 0;
	};
}