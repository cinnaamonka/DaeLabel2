#include "Font.h"

#include <SDL_ttf.h>
#include <stdexcept>

TTF_Font* GameEngine::Font::GetFont() const {
	return m_font;
}

GameEngine::Font::Font(const std::string& fullPath, unsigned int size) : m_font(nullptr)
{
	m_font = TTF_OpenFont(fullPath.c_str(), size);
	if (m_font == nullptr) 
	{
		throw std::runtime_error(std::string("Failed to load font: ") + SDL_GetError());
	}
}

GameEngine::Font::~Font()
{
	TTF_CloseFont(m_font);
}
