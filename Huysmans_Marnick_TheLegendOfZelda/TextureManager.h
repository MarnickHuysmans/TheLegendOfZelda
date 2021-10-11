#pragma once
#include "Texture.h"
#include <vector>

class TextureManager final
{
public:
	TextureManager() = default;
	~TextureManager();
	
	TextureManager(const TextureManager& other) = delete;
	TextureManager(TextureManager&& other) noexcept = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
	TextureManager& operator=(TextureManager&& other) noexcept = delete;
	
	Texture* CreateTexture(const std::string& str);
private:
	std::vector<Texture*> m_Textures;

	static bool Exists(const std::string& fileName);
};

