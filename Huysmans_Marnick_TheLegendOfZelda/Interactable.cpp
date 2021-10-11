#include "pch.h"
#include "Interactable.h"
#include "utils.h"

Interactable::Interactable(const Rectf& boundaries, const int destroyFrames, const Point2f&& destroyOffset, const int framesPerSec, const float fallSpeed, const float throwSpeed, const float flyingOffset, const int damage, const bool ground)
	: Projectile(damage, Direction::north, Vector2f{ 0.0f, 0.0f }, ground)
	, m_StartBoundaries(boundaries)
	, m_Boundaries(boundaries)
	, m_DestroyFrames(destroyFrames)
	, m_DestroyOffset(destroyOffset)
	, m_NrFramesPerSec(framesPerSec)
	, m_FallSpeed(fallSpeed)
	, m_ThrowSpeed(throwSpeed)
	, m_FlyingOffset(flyingOffset)
	, m_Destroyed(false)
	, m_PickUp(false)
	, m_Throwing(false)
	, m_Destroying(false)
	, m_AnimTime(0.0f)
	, m_AnimFrame(0)
	, m_CurrentHeight(0.0f)
{
}

void Interactable::DrawMap() const
{
	if (!m_PickUp && !m_Throwing)
	{
		Draw();
	}
}

void Interactable::DrawPlayer() const
{
	if (m_PickUp)
	{
		Draw();
	}
}

bool Interactable::Update(float elapsedSec, const Map* const map)
{
	if (m_Throwing && !m_Destroying)
	{
		return Flying(elapsedSec, map);
	}
	if (m_Destroying)
	{
		return Destroying(elapsedSec);
	}
	return true;
}

bool Interactable::Interact(const Rectf& actorShape, Direction direction, PickUpManager& pickUpManager)
{
	if (m_Destroyed || m_Destroying || m_Throwing)
	{
		return false;
	}
	bool hor{ false };
	bool ver{ false };
	float centerX{ actorShape.left + actorShape.width / 2 };
	float centerY{ actorShape.bottom + actorShape.height / 2 };
	switch (direction)
	{
	case Direction::north:
		ver = actorShape.bottom + actorShape.height == m_Boundaries.bottom;
		hor = centerX >= m_Boundaries.left && centerX <= m_Boundaries.left + m_Boundaries.width;
		break;
	case Direction::east:
		hor = actorShape.left + actorShape.width == m_Boundaries.left;
		ver = centerY >= m_Boundaries.bottom && centerY <= m_Boundaries.bottom + m_Boundaries.height;
		break;
	case Direction::south:
		ver = actorShape.bottom == m_Boundaries.bottom + m_Boundaries.height;
		hor = centerX >= m_Boundaries.left && centerX <= m_Boundaries.left + m_Boundaries.width;
		break;
	case Direction::west:
		hor = actorShape.left == m_Boundaries.left + m_Boundaries.width;
		ver = centerY >= m_Boundaries.bottom && centerY <= m_Boundaries.bottom + m_Boundaries.height;
		break;
	}
	if (hor && ver)
	{
		m_PickUp = true;
		DropPickUp(pickUpManager);
		return true;
	}
	return false;
}

bool Interactable::SwordHit(const std::vector<Point2f>& hitBox, PickUpManager& pickUpManager)
{
	return false;
}

void Interactable::Throw(Direction direction, float height)
{
	m_Throwing = true;
	m_PickUp = false;
	Vector2f velocity{};
	switch (direction)
	{
	case Direction::north:
		velocity.y = m_ThrowSpeed;
		break;
	case Direction::east:
		velocity.x = m_ThrowSpeed;
		break;
	case Direction::south:
		velocity.y = -m_ThrowSpeed;
		break;
	case Direction::west:
		velocity.x = -m_ThrowSpeed;
		break;
	}
	m_Direction = direction;
	m_Velocity = velocity;
	m_CurrentHeight = height;
}

void Interactable::Destroying()
{
	m_Destroying = true;
	m_CurrentHeight = 0.0f;
}

void Interactable::Destroy()
{
	m_Destroyed = true;
}

void Interactable::Reset()
{
	m_Boundaries = m_StartBoundaries;
	m_AnimTime = 0.0f;
	m_AnimFrame = 0;
	m_CurrentHeight = 0.0f;
	m_Destroyed = false;
	m_Destroying = false;
	m_PickUp = false;
	m_Throwing = false;
}

void Interactable::HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const
{
	if (m_Destroying || m_Destroyed || m_PickUp || m_Throwing)
	{
		return;
	}
	bool diagonal{ actorVelocity.x != 0.0f && actorVelocity.y != 0.0f };
	if (diagonal)
	{
		DiagonalMovement(actorShape, actorVelocity, collideXDiagonal, collideYDiagonal, collideXHor, collideYVer);
	}
	else
	{
		if (actorVelocity.x != 0.0f)
		{
			HorMovement(actorShape, actorVelocity, collideXHor);
		}
		else
		{
			VerMovement(actorShape, actorVelocity, collideYVer);
		}
	}
}

bool Interactable::Hit(const Rectf& hitBox)
{
	if (m_Destroyed || m_Destroying)
	{
		return false;
	}
	if (utils::IsOverlapping(hitBox, m_Boundaries))
	{
		Destroying();
		return true;
	}
	return false;
}

void Interactable::DiagonalMovement(Rectf& actorShape, Vector2f& actorVelocity, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const
{
	if (actorVelocity.x > 0)
	{
		if (actorVelocity.y > 0)
		{
			HitUpRight(actorShape, collideXDiagonal, collideYDiagonal, collideXHor, collideYVer);
		}
		else
		{
			HitDownRight(actorShape, collideXDiagonal, collideYDiagonal, collideXHor, collideYVer);
		}
	}
	else
	{
		if (actorVelocity.y > 0)
		{
			HitUpLeft(actorShape, collideXDiagonal, collideYDiagonal, collideXHor, collideYVer);
		}
		else
		{
			HitDownLeft(actorShape, collideXDiagonal, collideYDiagonal, collideXHor, collideYVer);
		}
	}
}

void Interactable::HorMovement(Rectf& actorShape, Vector2f& actorVelocity, float& collideXHor) const
{
	if (actorVelocity.x > 0)
	{
		HitRight(actorShape, collideXHor);
	}
	else
	{
		HitLeft(actorShape, collideXHor);
	}
}

void Interactable::VerMovement(Rectf& actorShape, Vector2f& actorVelocity, float& collideYVer) const
{
	if (actorVelocity.y > 0)
	{
		HitUp(actorShape, collideYVer);
	}
	else
	{
		HitDown(actorShape, collideYVer);
	}
}

void Interactable::HitUpRight(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const
{
	Point2f hitPoint{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideUp(actorShape, collideYDiagonal);
		CollideRight(actorShape, collideXDiagonal);
		return;
	}
	hitPoint = Point2f{ actorShape.left, actorShape.bottom + actorShape.height };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideUp(actorShape, collideYVer);
		return;
	}
	hitPoint = Point2f{ actorShape.left + actorShape.width, actorShape.bottom };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideRight(actorShape, collideXHor);
	}
}

void Interactable::HitDownRight(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const
{
	Point2f hitPoint{ actorShape.left + actorShape.width, actorShape.bottom };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideDown(collideYDiagonal);
		CollideRight(actorShape, collideXDiagonal);
		return;
	}
	hitPoint = Point2f{ actorShape.left, actorShape.bottom };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideDown(collideYVer);
		return;
	}
	hitPoint = Point2f{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideRight(actorShape, collideXHor);
	}
}

void Interactable::HitUpLeft(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const
{
	Point2f hitPoint{ actorShape.left, actorShape.bottom + actorShape.height };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideUp(actorShape, collideYDiagonal);
		CollideLeft(collideXDiagonal);
		return;
	}
	hitPoint = Point2f{ actorShape.left + actorShape.width, actorShape.bottom + actorShape.height };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideUp(actorShape, collideYVer);
		return;
	}
	hitPoint = Point2f{ actorShape.left, actorShape.bottom };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideLeft(collideXHor);
	}
}

void Interactable::HitDownLeft(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const
{
	Point2f hitPoint{ actorShape.left, actorShape.bottom };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideDown(collideYDiagonal);
		CollideLeft(collideXDiagonal);
		return;
	}
	hitPoint = Point2f{ actorShape.left + actorShape.width, actorShape.bottom };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideDown(collideYVer);
	}
	hitPoint = Point2f{ actorShape.left, actorShape.bottom + actorShape.height };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideLeft(collideXHor);
	}
}

void Interactable::HitRight(Rectf& actorShape, float& collideXHor) const
{
	Point2f hitPoint{ actorShape.left + actorShape.width, actorShape.bottom + m_VerOffset };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideRight(actorShape, collideXHor);
		return;
	}
	hitPoint.y = actorShape.bottom + actorShape.height - m_VerOffset;
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideRight(actorShape, collideXHor);
	}
}

void Interactable::HitLeft(Rectf& actorShape, float& collideXHor) const
{
	Point2f hitPoint{ actorShape.left, actorShape.bottom + m_VerOffset };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideLeft(collideXHor);
		return;
	}
	hitPoint.y = actorShape.bottom + actorShape.height - m_VerOffset;
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideLeft(collideXHor);
	}
}

void Interactable::HitUp(Rectf& actorShape, float& collideYVer) const
{
	Point2f hitPoint{ actorShape.left + m_HorOffset, actorShape.bottom + actorShape.height };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideUp(actorShape, collideYVer);
		return;
	}
	hitPoint.x = actorShape.left + actorShape.width - m_HorOffset;
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideUp(actorShape, collideYVer);
	}
}

void Interactable::HitDown(Rectf& actorShape, float& collideYVer) const
{
	Point2f hitPoint{ actorShape.left + m_HorOffset, actorShape.bottom };
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideDown(collideYVer);
		return;
	}
	hitPoint.x = actorShape.left + actorShape.width - m_HorOffset;
	if (utils::IsPointInRect(hitPoint, m_Boundaries))
	{
		CollideDown(collideYVer);
	}
}

void Interactable::CollideRight(Rectf& actorShape, float& collide) const
{
	collide = m_Boundaries.left - actorShape.width;
}

void Interactable::CollideLeft(float& collide) const
{
	collide = m_Boundaries.left + m_Boundaries.width;
}

void Interactable::CollideUp(Rectf& actorShape, float& collide) const
{
	collide = m_Boundaries.bottom - actorShape.height;
}

void Interactable::CollideDown(float& collide) const
{
	collide = m_Boundaries.bottom + m_Boundaries.height;
}

bool Interactable::Destroying(float elapsedSec)
{
	m_AnimTime += elapsedSec;
	float frameTime{ 1.0f / static_cast<float>(m_NrFramesPerSec) };
	if (m_AnimTime >= frameTime)
	{
		m_AnimTime -= frameTime;
		++m_AnimFrame;
		if (m_AnimFrame >= m_DestroyFrames)
		{
			m_AnimFrame = 0;
			m_AnimTime = 0.0f;
			m_Destroying = false;
			m_Throwing = false;
			Destroy();
			return true;
		}
	}
	return false;
}
