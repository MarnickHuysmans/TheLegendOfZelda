#include "pch.h"
#include "HUD.h"
#include "Player.h"
#include "utils.h"
#include "SoundManager.h"
#include "TextureManager.h"

HUD::HUD(const float scale, SoundManager& soundManager, TextureManager& textureManager)
	: m_Scale(scale)
	, m_SoundManager(soundManager)
	, m_Transition(false)
	, m_TransitionTimer(0.0f)
	, m_Volume(false)
	, m_Music(false)
	, m_Effects(false)
	, m_RupeeTimer(0.0f)
	, m_RupeeFrame(0)
	, m_Held(false)
{
	InitializeTextures(textureManager);
	InitializeSounds(soundManager);
}

bool HUD::Update(float elapsedSec)
{
	if (m_Transition)
	{
		m_TransitionTimer += elapsedSec;
		if (m_TransitionTimer >= m_TransitionTime)
		{
			m_TransitionTimer = 0.0f;
			m_Transition = false;
			m_Volume = !m_Volume;
			if (!m_Volume)
			{
				m_SoundManager.ResumeSoundEffects();
			}
		}
	}
	else if (m_Volume)
	{
		ProcessKeyBoardState();
	}
	if (m_Volume || m_Transition)
	{
		UpdateRupeeFrame(elapsedSec);
		return true;
	}
	return false;
}

void HUD::Draw(const Player& player) const
{
	glPushMatrix();
	glScalef(m_Scale, m_Scale, 1.0f);
	DrawGameHud(player);
	if (m_Volume || m_Transition)
	{
		DrawVolumeMenu();
	}
	glPopMatrix();
}

void HUD::Menu()
{
	if (!m_Transition)
	{
		m_Transition = true;
		m_Held = false;
		if (m_Volume)
		{
			m_Close->Play(0);
		}
		else
		{
			m_SoundManager.PauseSoundEffects();
			m_Open->Play(0);
		}
	}
}

void HUD::DrawGameHud(const Player& player) const
{
	if (m_HUDTexture)
	{
		m_HUDTexture->Draw(Point2f{});
	}
	DrawRupees(player.GetRupees());
	DrawHearts(player.GetHealth(), player.GetMaxHealth());
}

void HUD::DrawRupees(int rupees) const
{
	if (!m_NumbersTexture)
	{
		return;
	}
	int divide{ 1 };
	for (int i = 0; i < m_RupeeNumbers - 1; ++i)
	{
		divide *= 10;
	}

	for (int i = 0; i < m_RupeeNumbers; ++i)
	{
		int number{ rupees / divide % 10 };
		DrawRupeeNumber(number, i);
		divide /= 10;
	}
}

void HUD::DrawRupeeNumber(int number, int offset) const
{
	float width{ m_NumbersTexture->GetWidth() / static_cast<float>(m_NumbersAmount) };
	float height{ m_NumbersTexture->GetHeight() };

	float left{ width * static_cast<float>(number) };
	float bottom{ height };

	Point2f bottomLeft{ static_cast<float>(offset)* (width + m_IconOffset), 0.0f };
	bottomLeft += m_RupeesOffset;
	Rectf srcRect{ left, bottom, width, height };

	m_NumbersTexture->Draw(bottomLeft, srcRect);
}

void HUD::DrawHearts(int hearts, int maxHearts) const
{
	if (!m_HeartsTexture)
	{
		return;
	}

	for (int i = 0; i < maxHearts / 2; ++i)
	{
		if (hearts >= 2)
		{
			DrawHeart(0, i);
		}
		else if (hearts == 1)
		{
			DrawHeart(1, i);
		}
		else
		{
			DrawHeart(2, i);
		}
		hearts -= 2;
	}
}

void HUD::DrawHeart(int value, int offset) const
{
	float width{ m_HeartsTexture->GetWidth() / static_cast<float>(m_HeartsAmount) };
	float height{ m_HeartsTexture->GetHeight() };

	float left{ width * static_cast<float>(value) };
	float bottom{ height };

	int row{ offset / m_HeartCols };
	int col{ offset % m_HeartCols };

	Point2f bottomLeft{ static_cast<float>(col)* (width + m_IconOffset), static_cast<float>(row)* (-height - m_IconOffset) };
	bottomLeft += m_HeartsOffset;
	Rectf srcRect{ left, bottom, width, height };

	m_HeartsTexture->Draw(bottomLeft, srcRect);
}

void HUD::DrawVolumeMenu() const
{
	float offset{};
	if (m_Transition && m_VolumeTexture)
	{
		if (m_Volume)
		{
			offset = m_VolumeTexture->GetHeight() * m_TransitionTimer / m_TransitionTime;
		}
		else
		{
			offset = m_VolumeTexture->GetHeight() * (m_TransitionTime - m_TransitionTimer) / m_TransitionTime;
		}
	}
	DrawVolume(offset);
	DrawVolumeBars(offset);
}

void HUD::DrawVolume(float offset) const
{
	if (m_VolumeTexture)
	{
		m_VolumeTexture->Draw(Point2f{ 0.0f, offset });
	}
}

void HUD::DrawVolumeBars(float offset) const
{
	utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });

	float musicVolume{ static_cast<float>(m_SoundManager.GetSoundStreamVolume()) / m_VolumeFactor * m_VolumeSize };
	utils::FillRect(m_MusicLocation.x, m_MusicLocation.y + offset, musicVolume, m_VolumeBarHeight);

	float effectVolume{ static_cast<float>(m_SoundManager.GetSoundEffectVolume()) / m_VolumeFactor * m_VolumeSize };
	utils::FillRect(m_EffectsLocation.x, m_EffectsLocation.y + offset, effectVolume, m_VolumeBarHeight);

	if (m_Music)
	{
		DrawRupee(offset, musicVolume);
	}
	else
	{
		DrawRupee(offset, effectVolume);
	}
}

void HUD::DrawRupee(float offset, float volume) const
{
	int color{ static_cast<int>(volume / m_VolumeSize) / ((m_VolumeMax + 1) / m_RupeeNrColors) };

	float height{ m_RupeeTexture->GetHeight() / static_cast<float>(m_RupeeNrColors) };
	float bottom{ height * static_cast<float>(color + 1) };

	float width{ m_RupeeTexture->GetWidth() / static_cast<float>(m_RupeeNrOfFrames) };
	float left{ width * static_cast<float>(m_RupeeFrame) };

	Point2f bottomLeft{};
	if (m_Music)
	{
		bottomLeft = m_MusicLocation + m_RupeeOffset;
	}
	else
	{
		bottomLeft = m_EffectsLocation + m_RupeeOffset;
	}
	bottomLeft.y += offset;
	bottomLeft.x += volume;
	Rectf srcRect{ left, bottom, width, height };

	m_RupeeTexture->Draw(bottomLeft, srcRect);
}

void HUD::UpdateRupeeFrame(float elapsedSec)
{
	m_RupeeTimer += elapsedSec;
	float frameTime{ 1.0f / static_cast<float>(m_RupeeFramesPerSec) };
	if (m_RupeeTimer >= frameTime)
	{
		m_RupeeTimer -= frameTime;
		++m_RupeeFrame;
		m_RupeeFrame %= m_RupeeNrOfFrames;
	}
}

void HUD::ProcessKeyBoardState()
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	if (pStates[SDL_SCANCODE_W] || pStates[SDL_SCANCODE_S])
	{
		if (!m_Held)
		{
			m_Music = !m_Music;
			m_Effects = !m_Effects;
			m_Held = true;
		}
		return;
	}
	if (m_Held)
	{
		m_Held = false;
		return;
	}
	if (pStates[SDL_SCANCODE_A])
	{
		if (m_Music)
		{
			m_SoundManager.DecSoundStreamVolume();
		}
		else
		{
			m_SoundManager.DecSoundEffectVolume();
		}
		m_Held = true;
		m_Ping->Play(0);
		return;
	}
	if (pStates[SDL_SCANCODE_D])
	{
		if (m_Music)
		{
			m_SoundManager.IncSoundStreamVolume();
		}
		else
		{
			m_SoundManager.IncSoundEffectVolume();
		}
		m_Held = true;
		m_Ping->Play(0);
		return;
	}
	m_Held = false;
}

void HUD::InitializeTextures(TextureManager& textureManager)
{
	m_HUDTexture = textureManager.CreateTexture("Resources/HUD/HUD.png");
	m_HeartsTexture = textureManager.CreateTexture("Resources/HUD/Hearts.png");
	m_NumbersTexture = textureManager.CreateTexture("Resources/HUD/Numbers.png");
	m_VolumeTexture = textureManager.CreateTexture("Resources/HUD/Volume.png");
	m_RupeeTexture = textureManager.CreateTexture("Resources/PickUp/Rupee.png");
}

void HUD::InitializeSounds(SoundManager& soundManager)
{
	m_Open = soundManager.CreateSoundEffect("Resources/HUD/Open.wav");
	m_Close = soundManager.CreateSoundEffect("Resources/HUD/Close.wav");
	m_Ping = soundManager.CreateSoundEffect("Resources/HUD/Ping.wav");
}
