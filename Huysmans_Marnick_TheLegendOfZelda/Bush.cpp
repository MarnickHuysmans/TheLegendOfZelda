#include "pch.h"
#include "Bush.h"
#include "utils.h"
#include "Map.h"
#include "PickUpManager.h"
#include "Rupee.h"

const float Bush::m_Chance{ 25.0f };

Bush::Bush(const Rectf& boundaries, const bool ground)
	: Interactable(boundaries, 8, Point2f{ 8.0f, 13.0f }, 10, 50.0f, 200.0f, 0.5f, 1, ground)
{
}

void Bush::Draw() const
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

void Bush::SetPosition(Point2f& positionBottomCenter)
{
	m_Boundaries.left = positionBottomCenter.x - m_InteractableTexture->GetWidth() / 2;
	m_Boundaries.bottom = positionBottomCenter.y;
}

bool Bush::SwordHit(const std::vector<Point2f>& hitBox, PickUpManager& pickUpManager)
{
	if (!m_Destroyed && !m_Destroying && !m_Throwing && utils::IsOverlapping(hitBox, m_Boundaries))
	{
		RandomPickUp(pickUpManager);
		Destroying();
		return true;
	}
	return false;
}

void Bush::Destroying()
{
	if (m_DestroySound)
	{
		m_DestroySound->Play(0);
	}
	Interactable::Destroying();
}

void Bush::DropPickUp(PickUpManager& pickUpManager)
{
	RandomPickUp(pickUpManager);
}

bool Bush::Flying(float elapsedSec, const Map* const map)
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

void Bush::RandomPickUp(PickUpManager& pickUpManager)
{
	float chance{ static_cast<float>(rand() % 100 + 1) };
	if (chance <= m_Chance)
	{
		Point2f location{ m_Boundaries.left, m_Boundaries.bottom };
		location += m_PickUpOffset;
		Rupee* rupee{ new Rupee{location, Rupee::Color::green} };
		pickUpManager.AddPickUp(rupee);
	}
}
