#pragma once

#include "BaseComponent.h"

#include <string>

namespace GameEngine
{
	class Texture2D;
	class GameObject;

	class TextureComponent : public BaseComponent
	{
	public:
		TextureComponent(GameObject* GOptr, const std::string& filename, int textureClipAmount = 1);

		TextureComponent(GameObject* GOptr);

		std::shared_ptr<GameEngine::Texture2D> GetTexture() const;

		void SetTexture(const std::string& filename);
		void SetTexture(std::shared_ptr<GameEngine::Texture2D>& texture); 

		std::shared_ptr<GameEngine::Texture2D>& GetTexture()
		{
			return m_Texture;
		}
		
		int GetTextureClipAmount() const
		{
			return m_TextureClipAmount;
		}
	private:
		std::shared_ptr<GameEngine::Texture2D> m_Texture;
		std::string fileName;
		int m_TextureClipAmount;
	};
}


