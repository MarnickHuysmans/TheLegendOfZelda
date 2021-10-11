#pragma once
#include "Texture.h"
#include "Vector2f.h"
#include "SoundEffect.h"

class TextureManager;
class SoundManager;
class Player;

class HUD final
{
public:
	HUD(const float scale, SoundManager& soundManager, TextureManager& textureManager);

	HUD(const HUD& other) = delete;
	HUD(HUD&& other) noexcept = delete;
	HUD& operator=(const HUD& other) = delete;
	HUD& operator=(HUD&& other) noexcept = delete;

	bool Update(float elapsedSec);
	void Draw(const Player& player) const;

	void Menu();

private:
	const float m_IconOffset{ 1.0f };
	const Vector2f m_RupeesOffset{ 65.0f, 193.0f };
	const Vector2f m_HeartsOffset{ 161.0f, 193.0f };
	const Point2f m_MusicLocation{ 32.0f, 141.0f };
	const Point2f m_EffectsLocation{ 32.0f, 105.0f };
	const int m_RupeeNumbers{ 3 };
	const int m_HeartRows{ 2 };
	const int m_HeartCols{ 10 };
	const int m_NumbersAmount{ 10 };
	const int m_HeartsAmount{ 3 };
	const int m_VolumeMax{ 32 };
	const float m_VolumeFactor{ 4.0f };
	const float m_VolumeSize{ 6.0f };
	const float m_TransitionTime{ 0.5f };
	const int m_RupeeNrOfFrames{ 3 };
	const int m_RupeeNrColors{ 3 };
	const int m_RupeeFramesPerSec{ 5 };
	const float m_VolumeBarHeight{ 2.0f };
	const Vector2f m_RupeeOffset{ -3.0f, -6.0f };

	Texture* m_HUDTexture;
	Texture* m_HeartsTexture;
	Texture* m_NumbersTexture;
	Texture* m_VolumeTexture;
	Texture* m_RupeeTexture;
	float m_Scale;
	SoundManager& m_SoundManager;
	bool m_Transition;
	float m_TransitionTimer;
	bool m_Volume;
	bool m_Music;
	bool m_Effects;
	float m_RupeeTimer;
	int m_RupeeFrame;
	bool m_Held;

	SoundEffect* m_Open;
	SoundEffect* m_Close;
	SoundEffect* m_Ping;

	void DrawGameHud(const Player& player) const;
	void DrawRupees(int rupees) const;
	void DrawRupeeNumber(int number, int offset) const;
	void DrawHearts(int hearts, int maxHearts) const;
	void DrawHeart(int value, int offset) const;

	void DrawVolumeMenu() const;
	void DrawVolume(float offset) const;
	void DrawVolumeBars(float offset) const;
	void DrawRupee(float offset, float volume) const;

	void UpdateRupeeFrame(float elapsedSec);

	void ProcessKeyBoardState();

	void InitializeTextures(TextureManager& textureManager);
	void InitializeSounds(SoundManager& soundManager);
};
