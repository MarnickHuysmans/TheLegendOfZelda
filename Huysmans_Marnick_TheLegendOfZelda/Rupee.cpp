#include "pch.h"
#include "Rupee.h"
#include "Texture.h"
#include "Player.h"

int Rupee::m_NrOfColors{ 3 };


Rupee::Rupee(const Point2f& location, Color color)
	: PickUp(Rectf{ location.x, location.y, m_Size.x, m_Size.y })
	, m_Color(color)
{
}

void Rupee::Draw() const
{
	if (!m_Draw)
	{
		return;
	}
	int color{ static_cast<int>(m_Color) };

	float height{ m_Sprite->GetHeight() / static_cast<float>(m_NrOfColors) };
	float bottom{ height * static_cast<float>(color + 1) };

	float width{ m_Sprite->GetWidth() / static_cast<float>(m_NrOfFrames) };
	float left{ width * static_cast<float>(m_AnimFrame) };

	Point2f bottomLeft{ m_HitBox.left, m_HitBox.bottom };
	Rectf srcRect{ left, bottom, width, height };

	m_Sprite->Draw(bottomLeft, srcRect);
}

bool Rupee::Update(const float elapsedSec, Player& player)
{
	if (Overlap(player))
	{
		player.AddRupees(GetValue());
		return true;
	}
	if (m_FadeTimer >= m_FadeTime)
	{
		m_DestroyTimer += elapsedSec;
		m_Draw = !m_Draw;
		if (m_DestroyTimer >= m_DestroyTime)
		{
			return true;
		}
	}
	else
	{
		m_FadeTimer += elapsedSec;
	}
	m_AnimTime += elapsedSec;
	float frameTime{ 1.0f / static_cast<float>(m_FramesPerSec) };
	if (m_AnimTime >= frameTime)
	{
		m_AnimTime -= frameTime;
		++m_AnimFrame;
		m_AnimFrame %= m_NrOfFrames;
	}
	return false;
}

Rupee* Rupee::Copy()
{
	Point2f location{ m_HitBox.left, m_HitBox.bottom };
	return new Rupee{ location, m_Color };
}

bool Rupee::Overlap(Player& player)
{
	bool overlap{ PickUp::Overlap(player) };
	if (overlap && m_PickUpSound)
	{
		m_PickUpSound->Play(0);
	}
	return overlap;
}

int Rupee::GetValue() const
{
	switch (m_Color)
	{
	case Color::green:
		return 1;
	case Color::blue:
		return 5;
	case Color::red:
		return 20;
	}
	return 0;
}
