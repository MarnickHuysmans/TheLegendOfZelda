#pragma once
#include "Projectile.h"
#include <list>

class Transition;
class Player;

class ProjectileManager final
{
public:
	ProjectileManager();

	~ProjectileManager();
	ProjectileManager(const ProjectileManager& other) = delete;
	ProjectileManager(ProjectileManager&& other) noexcept = delete;
	ProjectileManager& operator=(const ProjectileManager& other) = delete;
	ProjectileManager& operator=(ProjectileManager&& other) noexcept = delete;

	void Update(float elapsedSec, Player& player, ProjectileManager& projectileManager);
	void DrawGround() const;
	void DrawUp() const;
	void AddPlayerProjectile(Projectile* projectile);
	void AddEnemyProjectile(Projectile* projectile);
	Projectile* HitEnemy(const Rectf& enemyHitBox, const bool ground);

	void RemoveProjectiles();

private:
	std::list<Projectile*> m_PlayerProjectiles;
	std::list<Projectile*> m_EnemyProjectiles;
};

