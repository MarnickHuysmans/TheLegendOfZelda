#include "pch.h"
#include "Pot.h"
#include "utils.h"
#include "Texture.h"
#include "Map.h"
#include "PickUpManager.h"
#include "SoundEffect.h"

Pot::Pot(const Rectf& boundaries, PickUp* pickUp, const bool ground)
	:Interactable(boundaries, 8, Point2f{ 8.0f, 13.0f }, 10, 50.0f, 200.0f, 0.5f, 1, ground)
	, m_PickUp(pickUp)
{
}

Pot::~Pot()
{
	delete m_PickUp;
}

void Pot::Draw() const
{
	if (!m_Destroyed && !m_Destroying)
	{
		m_InteractableTexture->Draw(m_Boundaries);

		if (m_Throwing)
		{
			float centerX{ m_Boundaries.left + m_Boundaries.width / 2 };
			Point2f bottomLeft{ centerX - m_Shadow->GetWidth() / 2, m_Boundaries.bottom - m_CurrentHeight };
			m_Shadow->Draw(bottomLeft);
		}

#if defined(DEBUG) | defined(_DEBUG)
		utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
		utils::DrawRect(m_Boundaries);
#endif
	}
	else if (m_Destroying)
	{
		float width{ m_Destroy->GetWidth() / m_DestroyFrames };
		float height{ m_Destroy->GetHeight() };

		float left{ width * static_cast<float>(m_AnimFrame) };
		float bottom{ height };

		Point2f bottomLeft{ m_Boundaries.left - m_DestroyOffset.x, m_Boundaries.bottom - m_DestroyOffset.y };
		Rectf srcRect{ left, bottom, width, height };
		m_Destroy->Draw(bottomLeft, srcRect);
	}
}

void Pot::SetPosition(Point2f& positionBottomCenter)
{
	m_Boundaries.left = positionBottomCenter.x - m_InteractableTexture->GetWidth() / 2;
	m_Boundaries.bottom = positionBottomCenter.y;
}

void Pot::Destroying()
{
	if (m_DestroySound)
	{
		m_DestroySound->Play(0);
	}
	Interactable::Destroying();
}

bool Pot::Flying(float elapsedSec, const Map* map)
{
	float heightDif{ elapsedSec * m_FallSpeed };
	if (m_Velocity.x != 0.0f)
	{
		m_Boundaries.bottom -= heightDif;
	}
	m_CurrentHeight -= heightDif;
	m_Boundaries.bottom += elapsedSec * m_Velocity.y;
	m_Boundaries.left += elapsedSec * m_Velocity.x;

	Rectf hitBox{ m_Boundaries };

	float leftOffset{ (hitBox.width - m_Shadow->GetWidth()) / 2.0f + m_FlyingOffset };
	hitBox.width = m_Shadow->GetWidth() - m_FlyingOffset * 2;
	hitBox.left += leftOffset;

	hitBox.height = m_Shadow->GetHeight();
	hitBox.bottom -= m_CurrentHeight;
	hitBox.bottom += m_FlyingOffset;
	hitBox.height -= m_FlyingOffset * 2;

	if (m_CurrentHeight <= 0.0f || map->CheckOverlap(hitBox, m_Ground))
	{
		Destroying();
	}
	return false;
}

void Pot::DropPickUp(PickUpManager& pickUpManager)
{
	pickUpManager.AddPickUp(m_PickUp->Copy());
}
