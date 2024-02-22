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

	// Game object background

	auto gameObject = std::make_shared<dae::GameObject>();
	
	// transform component to background
	auto backgroundtransformComponent = std::make_unique<dae::TransformComponent>(glm::vec3(0, 0, 0));
	auto& addedBackgroundTransformComponent = gameObject->AddComponent<dae::TransformComponent>(backgroundtransformComponent);
	addedBackgroundTransformComponent.SetGameObject(gameObject.get());

	// texture component to background
	auto textureComponent = std::make_unique<dae::TextureComponent>("background.tga");
	auto& addedBackgroundTextureComponent = gameObject->AddComponent<dae::TextureComponent>(textureComponent);
	addedBackgroundTextureComponent.SetGameObject(gameObject.get());

	// render component to background
	auto renderComponent = std::make_unique<dae::RenderComponent>();
	auto& addedRenderComponent = gameObject->AddComponent<dae::RenderComponent>(renderComponent);
	addedRenderComponent.SetGameObject(gameObject.get());

	scene.Add(gameObject);

	// game object DAELabel

	gameObject = std::make_shared<dae::GameObject>();

	// transform component to DAELabel
	auto logoTransformComponent = std::make_unique<dae::TransformComponent>(glm::vec3(216, 180, 0));  
	auto& addedLogoTransformComponent = gameObject->AddComponent<dae::TransformComponent>(logoTransformComponent);  
	addedLogoTransformComponent.SetGameObject(gameObject.get());

	// add texture component to DAELabel
	auto logoTextureComponent = std::make_unique<dae::TextureComponent>("logo.tga");
	auto& addedLogoTextureComponent = gameObject->AddComponent<dae::TextureComponent>(logoTextureComponent); 
	addedLogoTextureComponent.SetGameObject(gameObject.get());

	// add render component to DAELAbel
	auto logoRenderComponent = std::make_unique<dae::RenderComponent>();  
	auto& addedLogoRenderComponent = gameObject->AddComponent<dae::RenderComponent>(logoRenderComponent);  
	addedLogoRenderComponent.SetGameObject(gameObject.get());

	scene.Add(gameObject);  

	// add text object 
	auto font = dae::ResourceManager::GetInstance().LoadFont("Lingua.otf", 24);
	gameObject = std::make_shared<dae::GameObject>();

	// add transform component to this game object
	auto textTransformComponent = std::make_unique<dae::TransformComponent>(glm::vec3(170, 20, 0));
	auto& addedTextTransformComponent = gameObject->AddComponent<dae::TransformComponent>(textTransformComponent);
	addedTextTransformComponent.SetGameObject(gameObject.get());

	// add text component 
	auto textComponent = std::make_unique<dae::TextComponent>("Programming 4 Assignment", font);
	auto& addedTextComponent = gameObject->AddComponent<dae::TextComponent>(textComponent);
	addedTextComponent.SetGameObject(gameObject.get());

	// add render component
	auto textRenderComponent = std::make_unique<dae::RenderComponent>();
	auto& addedtextTenderComponent = gameObject->AddComponent<dae::RenderComponent>(textRenderComponent);
	addedtextTenderComponent.SetGameObject(gameObject.get());

	scene.Add(gameObject);

	// add fps game object

	gameObject = std::make_shared<dae::GameObject>();

	// add fps component
	auto FPSComponent = std::make_unique<dae::FPS>();
	auto& addedFPSComponent = gameObject->AddComponent<dae::FPS>(FPSComponent);
	addedFPSComponent.SetGameObject(gameObject.get());

	//add transform component
	auto FPSTransformComponent = std::make_unique<dae::TransformComponent>(glm::vec3(20, 20, 0));
	auto& addedFPSTransformComponent = gameObject->AddComponent<dae::TransformComponent>(FPSTransformComponent);
	addedFPSTransformComponent.SetGameObject(gameObject.get());

	// add text component 
	auto textFPSComponent = std::make_unique<dae::TextComponent>("FPS", font);
	auto& addedFPSTextComponent = gameObject->AddComponent<dae::TextComponent>(textFPSComponent);
	addedFPSTextComponent.SetGameObject(gameObject.get());

	// add render component
	auto FPSRenderComponent = std::make_unique<dae::RenderComponent>();
	auto& addedFPSRenderComponent = gameObject->AddComponent<dae::RenderComponent>(FPSRenderComponent);
	addedFPSRenderComponent.SetGameObject(gameObject.get());

	scene.Add(gameObject);
}

int main(int, char*[]) {
	dae::Minigin engine("../Data/");
	engine.Run(load);
    return 0;
}