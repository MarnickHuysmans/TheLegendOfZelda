#include "pch.h"
#include "Heart.h"
#include "Player.h"

Heart::Heart(const Point2f& location)
	: PickUp(Rectf{ location.x, location.y, m_Size.x, m_Size.y })
{
}

void Heart::Draw() const
{
	if (!m_Draw)
	{
		return;
	}

	Point2f bottomLeft{ m_HitBox.left, m_HitBox.bottom };
	m_Sprite->Draw(bottomLeft);
}

bool Heart::Update(const float elapsedSec, Player& player)
{
	if (Overlap(player))
	{
		player.AddHealth(GetValue());
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

Heart* Heart::Copy()
{
	Point2f location{ m_HitBox.left, m_HitBox.bottom };
	return new Heart{ location };
}

bool Heart::Overlap(Player& player)
{
	bool overlap{ PickUp::Overlap(player) };
	if (overlap && m_PickUpSound)
	{
		m_PickUpSound->Play(0);
	}
	return overlap;
}

int Heart::GetValue() const
{
	return m_Value;
}
