#include "pch.h"
#include "TextureManager.h"
#include <fstream>

TextureManager::~TextureManager()
{
	for (Texture* texture : m_Textures)
	{
		delete texture;
	}
}

Texture* TextureManager::CreateTexture(const std::string& str)
{
	if (!Exists(str))
	{
		return nullptr;
	}
	Texture* texture{ new Texture{str} };
	if (texture->IsCreationOk())
	{
		m_Textures.push_back(texture);
		return texture;
	}
	delete texture;
	return nullptr;
}

bool TextureManager::Exists(const std::string& fileName)
{
	std::ifstream file{ fileName };
	return file.good();
}
