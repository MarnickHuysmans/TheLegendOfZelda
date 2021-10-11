#pragma once
#include "Direction.h"
#include "Vector2f.h"
class Player;
class Map;

class Projectile
{
public:
	Projectile(const int damage, const Direction direction, const Vector2f&& velocity, const bool ground);
	virtual ~Projectile() {}

	virtual bool Update(float elapsedSec, const Map* map) = 0;
	virtual void Draw() const = 0;
	virtual bool Hit(const Rectf& hitBox) = 0;
	virtual int GetDamage();
	virtual Direction GetDirection();
	virtual void SetGround(const bool ground);
	virtual  bool IsGround() const;

protected:
	const int m_Damage;
	
	Direction m_Direction;
	Vector2f m_Velocity;
	bool m_Ground;

	virtual bool Flying(float elapsedSec, const Map* map) = 0;
};

