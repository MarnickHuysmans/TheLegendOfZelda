#pragma once
#include "Texture.h"
#include "SoundStream.h"
class TextureManager;
class Level;
class SoundManager;

class Title final
{
public:
	Title(const float scale, SoundManager& soundManager, TextureManager& textureManager);

	Title(const Title& other) = delete;
	Title(Title&& other) noexcept = delete;
	Title& operator=(const Title& other) = delete;
	Title& operator=(Title&& other) noexcept = delete;

	bool Update(float elapsedSec);
	bool Draw() const;

	void PlayMusic();
	bool KeyStroke(const Level* level);

private:
	const float m_TriForceTime{ 6.0f };
	const float m_SwordTime{ 1.0f };
	const float m_Scale;

	bool m_Active;
	float m_AnimTime;
	bool m_TriForce;
	bool m_Sword;

	Texture* m_BackgroundTexture;
	Texture* m_SwordTexture;
	Texture* m_TitleTexture;
	Texture* m_TitleOverlayTexture;
	Texture* m_TriForceTexture;

	SoundStream* m_Start;
	SoundStream* m_Loop;

	bool UpdateAnimation(float time, float elapsedSec);

	void DrawBackground() const;
	void DrawTriForce() const;
	void DrawTitle() const;
	void DrawSword() const;
	void DrawTitleOverlay() const;

	void PlayLoop();

	void InitializeTextures(TextureManager& textureManager);
	void InitializeSounds(SoundManager& soundManager);
};

