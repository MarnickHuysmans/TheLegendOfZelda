#include "pch.h"
#include "Stairs.h"
#include "utils.h"

Stairs::Stairs(const Rectf& boundaries, const bool ground)
	: m_Boundaries(boundaries)
	, m_Ground(ground)
{

}

bool Stairs::TakeStairs(const Rectf& playerRect)
{
	return utils::IsOverlapping(playerRect, m_Boundaries);
}

bool Stairs::IsGround() const
{
	return m_Ground;
}

Rectf Stairs::GetBoundaries() const
{
	return m_Boundaries;
}
