#include <SDL.h>

#if _DEBUG
// ReSharper disable once CppUnusedIncludeDirective
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "Components.h"
#include "TextObject.h"

#include "Scene.h"

#include <memory>

void load()
{
	auto& scene = dae::SceneManager::GetInstance().CreateScene("Demo");

	// game object 1

	auto gameObject = std::make_shared<dae::GameObject>();
	
	// texture component
	auto textureComponent = std::make_unique<dae::TextureComponent>();
	textureComponent->SetTexture("background.tga");

	auto& added = gameObject->AddComponent<dae::TextureComponent>("TextureComponent",textureComponent);
	
	// render component
	auto renderComponent = std::make_unique<dae::RenderComponent>(added.GetTexture(), glm::vec3(0, 0, 0));
	gameObject->AddComponent<dae::RenderComponent>("RenderComponent", renderComponent);

	scene.Add(gameObject);

	// game object 2

	gameObject = std::make_shared<dae::GameObject>();

	// transform component

	auto logoTransformComponent = std::make_unique<dae::TransformComponent>(glm::vec3(216, 180, 0));
	auto& addedLogoTransformComponent = gameObject->AddComponent<dae::TransformComponent>("TransformComponent", logoTransformComponent);

	// texture component
	auto logoTextureComponent = std::make_unique<dae::TextureComponent>();

	logoTextureComponent->SetTexture("logo.tga");

	auto& addedComponent = gameObject->AddComponent<dae::TextureComponent>("TextureComponent", logoTextureComponent);
	
	// render component
	auto logoRenderComponent = std::make_unique<dae::RenderComponent>(addedComponent.GetTexture(), addedLogoTransformComponent.GetPosition());

	gameObject->AddComponent<dae::RenderComponent>("RenderComponent", logoRenderComponent);

	scene.Add(gameObject);

	// game object 3
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
	gameObject = std::make_shared<dae::GameObject>();

	// add transform component to this game object
	auto textTransformComponent = std::make_unique<dae::TransformComponent>(glm::vec3(170, 20, 0));
	auto& addedTextTransformComponent = gameObject->AddComponent<dae::TransformComponent>("TransformComponent", textTransformComponent);

	// add text component 
	auto textComponent = std::make_unique<dae::TextComponent>("Programming 4 Assignment", font);
	auto& addedTextComponent = gameObject->AddComponent<dae::TextComponent>("TextComponent", textComponent);

	// add render component
	auto textRenderComponent = std::make_unique<dae::RenderComponent>(addedTextComponent.GetTexturePtr(), addedTextTransformComponent.GetPosition());

	gameObject->AddComponent<dae::RenderComponent>("RenderComponent", textRenderComponent);

	scene.Add(gameObject);

	// game object 4

	gameObject = std::make_shared<dae::GameObject>();

	// add fps component
	auto FPSComponent = std::make_unique<dae::FPS>();
	auto& addedFPSComponent = gameObject->AddComponent<dae::FPS>("FPS", FPSComponent);

	//add transform component
	auto FPSTransformComponent = std::make_unique<dae::TransformComponent>(glm::vec3(20, 20, 0));
	auto& addedFPSTransformComponent = gameObject->AddComponent<dae::TransformComponent>("TransformComponent", FPSTransformComponent);

	// add text component 
	auto textFPSComponent = std::make_unique<dae::TextComponent>(addedFPSComponent.GetFPSPtr(), font);
	auto& addedFPSTextComponent = gameObject->AddComponent<dae::TextComponent>("TextComponent", textFPSComponent);

	// add render component
	auto FPSRenderComponent = std::make_unique<dae::RenderComponent>(addedFPSTextComponent.GetTexturePtr(), addedFPSTransformComponent.GetPosition());

	gameObject->AddComponent<dae::RenderComponent>("RenderComponent", FPSRenderComponent);

	scene.Add(gameObject);
}

int main(int, char*[]) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
    return 0;
}