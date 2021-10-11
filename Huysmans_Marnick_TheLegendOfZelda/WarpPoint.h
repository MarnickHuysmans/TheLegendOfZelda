#pragma once
#include "Direction.h"

class Map;

class WarpPoint final
{
public:
	WarpPoint(const Rectf& boundaries, Map* map, const Point2f warpLocation, const bool ground, const Direction direction);
	bool Warp(const Rectf& playerRect) const;
	Map* GetMap() const;
	Point2f GetLocation() const;
	Rectf GetBoundaries() const;
	bool IsGround() const;
	Direction GetDirection() const;
private:
	Rectf m_Boundaries;
	Map* m_Map;
	Point2f m_WarpLocation;
	bool m_Ground;
	Direction m_Direction;
};

