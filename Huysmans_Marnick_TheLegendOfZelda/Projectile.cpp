#include "pch.h"
#include "Projectile.h"

Projectile::Projectile(const int damage, const Direction direction, const Vector2f&& velocity, const bool ground)
	: m_Damage(damage)
	, m_Direction(direction)
	, m_Velocity(velocity)
	, m_Ground(ground)
{
}

int Projectile::GetDamage()
{
	return m_Damage;
}

Direction Projectile::GetDirection()
{
	return m_Direction;
}

void Projectile::SetGround(const bool ground)
{
	m_Ground = ground;
}

bool Projectile::IsGround() const
{
	return m_Ground;
}
