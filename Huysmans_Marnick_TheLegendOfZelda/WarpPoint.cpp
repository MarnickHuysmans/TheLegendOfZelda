#include "pch.h"
#include "WarpPoint.h"
#include "utils.h"

WarpPoint::WarpPoint(const Rectf& boundaries, Map* map, const Point2f warpLocation, const bool ground, const Direction direction)
	: m_Boundaries(boundaries)
	, m_Map(map)
	, m_WarpLocation(warpLocation)
	, m_Ground(ground)
	, m_Direction(direction)
{
}

bool WarpPoint::Warp(const Rectf& playerRect) const
{
	return utils::IsOverlapping(playerRect, m_Boundaries);
}

Map* WarpPoint::GetMap() const
{
	return m_Map;
}

Point2f WarpPoint::GetLocation() const
{
	return m_WarpLocation;
}

Rectf WarpPoint::GetBoundaries() const
{
	return m_Boundaries;
}

bool WarpPoint::IsGround() const
{
	return m_Ground;
}

Direction WarpPoint::GetDirection() const
{
	return m_Direction;
}
