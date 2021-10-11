#include "pch.h"
#include "Title.h"
#include "SoundManager.h"
#include "TextureManager.h"
#include <algorithm>
#include "utils.h"
#include "Level.h"

Title::Title(const float scale, SoundManager& soundManager, TextureManager& textureManager)
	: m_Scale(scale)
	, m_Active(true)
	, m_AnimTime(0.0f)
	, m_TriForce(true)
	, m_Sword(false)
{
	InitializeTextures(textureManager);
	InitializeSounds(soundManager);
}

bool Title::Update(float elapsedSec)
{
	if (!m_Active)
	{
		return false;
	}
	PlayLoop();

	if (m_TriForce)
	{
		if (UpdateAnimation(m_TriForceTime, elapsedSec))
		{
			m_TriForce = false;
			m_Sword = true;
		}
	}
	else if (m_Sword)
	{
		if (UpdateAnimation(m_SwordTime, elapsedSec))
		{
			m_Sword = false;
		}
	}
	return true;
}

bool Title::Draw() const
{
	if (!m_Active)
	{
		return false;
	}
	glPushMatrix();
	glScalef(m_Scale, m_Scale, 1.0f);
	DrawBackground();
	DrawTriForce();
	DrawTitle();
	DrawSword();
	DrawTitleOverlay();
	glPopMatrix();
	return true;
}

bool Title::UpdateAnimation(float time, float elapsedSec)
{
	m_AnimTime += elapsedSec;
	if (m_AnimTime >= time)
	{
		m_AnimTime -= time;
		return true;
	}
	return false;
}

void Title::DrawBackground() const
{
	if (!m_TriForce)
	{
		if (m_BackgroundTexture)
		{
			m_BackgroundTexture->Draw();
		}
	}
}

void Title::DrawTriForce() const
{
	if (m_TriForceTexture)
	{
		m_TriForceTexture->Draw();
		if (m_TriForce)
		{
			float opacity{ 1.0f - m_AnimTime / m_TriForceTime };
			utils::SetColor(Color4f{ 0.0f,0.0f,0.0f, opacity });
			utils::FillRect(Point2f{}, m_TriForceTexture->GetWidth(), m_TriForceTexture->GetHeight());
		}
	}
}

void Title::DrawTitle() const
{
	if (!m_TriForce)
	{
		if (m_TitleTexture)
		{
			m_TitleTexture->Draw();
		}
	}
}

void Title::DrawSword() const
{
	if (!m_TriForce)
	{
		if (m_SwordTexture)
		{
			Point2f bottomLeft{};
			if (m_Sword)
			{
				float height{ m_SwordTexture->GetHeight() };
				bottomLeft.y = height - height * (m_AnimTime / m_SwordTime);
			}
			m_SwordTexture->Draw(bottomLeft);
		}
	}
}

void Title::DrawTitleOverlay() const
{
	if (!m_TriForce)
	{
		if (m_TitleOverlayTexture)
		{
			m_TitleOverlayTexture->Draw();
		}
	}
}

bool Title::KeyStroke(const Level* level)
{
	if (m_Active)
	{
		m_Active = false;
		level->PlayMusic();
		return true;
	}
	return false;
}

void Title::PlayMusic()
{
	m_Start->Play(false);
}

void Title::PlayLoop()
{
	if (!SoundStream::IsPlaying())
	{
		m_Loop->Play(true);
	}
}

void Title::InitializeTextures(TextureManager& textureManager)
{
	m_BackgroundTexture = textureManager.CreateTexture("Resources/Title/Background.png");
	m_SwordTexture = textureManager.CreateTexture("Resources/Title/Sword.png");
	m_TitleTexture = textureManager.CreateTexture("Resources/Title/Title.png");
	m_TitleOverlayTexture = textureManager.CreateTexture("Resources/Title/TitleOverlay.png");
	m_TriForceTexture = textureManager.CreateTexture("Resources/Title/TriForce.png");
}

void Title::InitializeSounds(SoundManager& soundManager)
{
	m_Loop = soundManager.CreateSoundStream("Resources/Title/Loop.mp3");
	m_Start = soundManager.CreateSoundStream("Resources/Title/Start.mp3");
}
