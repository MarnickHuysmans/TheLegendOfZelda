#include "pch.h"
#include "Camera.h"
#include "Map.h"

Camera::Camera(float width, float height, float scale)
	: m_Width(width)
	, m_Height(height)
	, m_Scale(scale)
	, m_FreeMovement(Rectf{ 0.0f, 0.0f, width / scale / 8.0f, height / scale / 8.0f })
{
}

Rectf Camera::CameraTransform() const
{
	glScalef(m_Scale, m_Scale, 1.0f);
	Point2f location{ CameraBottomLeft() };
	glTranslatef(-location.x, -location.y, 0.0f);
	return Rectf{ location.x, location.y, m_Width / m_Scale, m_Height / m_Scale };
}

void Camera::Transform(const Rectf& target, const Map* const map, const bool free)
{
	if (free)
	{
		Free(target, map);
	}
	else
	{
		CenterOn(target, map);
	}
}

void Camera::CenterOn(const Rectf& target, const Map* const map)
{
	Point2f position{ Track(target) };
	Clamp(position, map);
	FreeFromCameraBottomLeft(position);
}

Point2f Camera::Track(const Rectf& target)
{
	float left{ target.left + target.width / 2.0f - m_Width / m_Scale / 2.0f };
	float bottom{ target.bottom + target.height / 2.0f - m_Height / m_Scale / 2.0f };
	return Point2f{ left, bottom };
}

bool Camera::Clamp(Point2f& bottomLeftPos, const Map* const map) const
{
	Rectf mapBoundaries = map->GetBoundaries();
	bool clamp{ false };

	bool north{ true };
	if (!map->GetSouth())
	{
		if (bottomLeftPos.y < mapBoundaries.bottom)
		{
			bottomLeftPos.y = mapBoundaries.bottom;
			north = false;
			clamp = true;
		}
	}

	if (north)
	{
		if (!map->GetNorth())
		{
			if (bottomLeftPos.y + m_Height / m_Scale > mapBoundaries.bottom + mapBoundaries.height)
			{
				bottomLeftPos.y = mapBoundaries.bottom + mapBoundaries.height - m_Height / m_Scale;
				clamp = true;
			}
		}
	}

	bool east{ true };
	if (!map->GetWest())
	{
		if (bottomLeftPos.x < mapBoundaries.left)
		{
			bottomLeftPos.x = mapBoundaries.left;
			east = false;
			clamp = true;
		}
	}

	if (east)
	{
		if (!map->GetEast())
		{
			if (bottomLeftPos.x + m_Width / m_Scale > mapBoundaries.left + mapBoundaries.width)
			{
				bottomLeftPos.x = mapBoundaries.left + mapBoundaries.width - m_Width / m_Scale;
				clamp = true;
			}
		}
	}
	return clamp;
}

void Camera::Free(const Rectf& target, const Map* map)
{
	if (target.left >= m_FreeMovement.left && target.bottom >= m_FreeMovement.bottom && target.left + target.width <= m_FreeMovement.left + m_FreeMovement.width && target.bottom + target.height <= m_FreeMovement.bottom + m_FreeMovement.height)
	{
		return;
	}
	MoveFreeTransform(target);
	ClampFree(map);
}

void Camera::MoveFreeTransform(const Rectf& target)
{
	if (target.left < m_FreeMovement.left)
	{
		m_FreeMovement.left = target.left;
	}
	else if (target.left + target.width > m_FreeMovement.left + m_FreeMovement.width)
	{
		m_FreeMovement.left = target.left + target.width - m_FreeMovement.width;
	}
	if (target.bottom < m_FreeMovement.bottom)
	{
		m_FreeMovement.bottom = target.bottom;
	}
	else if (target.bottom + target.height > m_FreeMovement.bottom + m_FreeMovement.height)
	{
		m_FreeMovement.bottom = target.bottom + target.height - m_FreeMovement.height;
	}
}

void Camera::ClampFree(const Map* const map)
{
	Point2f bottomLeft{ CameraBottomLeft() };
	if (Clamp(bottomLeft, map))
	{
		FreeFromCameraBottomLeft(bottomLeft);
	}
}

Point2f Camera::CameraBottomLeft() const
{
	float left{ m_FreeMovement.left + m_FreeMovement.width / 2.0f - m_Width / m_Scale / 2.0f };
	float bottom{ m_FreeMovement.bottom + m_FreeMovement.height / 2.0f - m_Height / m_Scale / 2.0f };
	return Point2f{ left, bottom };
}

void Camera::FreeFromCameraBottomLeft(const Point2f& bottomLeft)
{
	m_FreeMovement.left = bottomLeft.x + m_Width / m_Scale / 2.0f - m_FreeMovement.width / 2.0f;
	m_FreeMovement.bottom = bottomLeft.y + m_Height / m_Scale / 2.0f - m_FreeMovement.height / 2.0f;
}
