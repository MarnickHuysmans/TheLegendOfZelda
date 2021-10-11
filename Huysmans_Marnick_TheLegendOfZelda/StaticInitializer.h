#pragma once
class SoundEffect;
class SoundManager;
class Texture;
class TextureManager;

class StaticInitializer final
{
public:
	static void InitStatics(TextureManager& textureManager, SoundManager& soundManager);
	
private:
	static std::string m_ResourcesFolder;

	static void InitInteractables(TextureManager& textureManager, SoundManager& soundManager);
	static void InitBush(TextureManager& textureManager, Texture* shadow, SoundManager& soundManager);
	static void InitPot(TextureManager& textureManager, Texture* shadow, SoundManager& soundManager);

	static void InitPickUps(TextureManager& textureManager, SoundManager& soundManager);
	static void InitRupee(TextureManager& textureManager, SoundManager& soundManager);
	static void InitHeart(TextureManager& textureManager, SoundManager& soundManager);

	static void InitEnemies(TextureManager& textureManager, SoundManager& soundManager);
	static void InitSwordSoldier(TextureManager& textureManager, Texture* death, SoundEffect* soundEffect, SoundManager& soundManager);
	static void InitRat(TextureManager& textureManager, Texture* death, SoundEffect* soundEffect, SoundManager& soundManager);
};

