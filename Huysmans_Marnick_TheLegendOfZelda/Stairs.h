#pragma once
class Stairs final
{
public:
	Stairs(const Rectf& boundaries, const bool ground);
	bool TakeStairs(const Rectf& playerRect);
	bool IsGround() const;
	Rectf GetBoundaries() const;
private:
	Rectf m_Boundaries;
	bool m_Ground;
};

