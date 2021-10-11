#include "pch.h"
#include "ProjectileManager.h"
#include "Interactable.h"
#include "Player.h"

ProjectileManager::ProjectileManager()
{
}

ProjectileManager::~ProjectileManager()
{
	RemoveProjectiles();
}

void ProjectileManager::Update(float elapsedSec, Player& player, ProjectileManager& projectileManager)
{
	Map* map{ player.GetMap() };
	auto it{ m_PlayerProjectiles.begin() };
	while (it != m_PlayerProjectiles.end())
	{
		if ((*it)->Update(elapsedSec, map))
		{
			if (!dynamic_cast<Interactable*>(*it))
			{
				delete* it;
			}
			it = m_PlayerProjectiles.erase(it);
		}
		else
		{
			++it;
		}
	}
	it = m_EnemyProjectiles.begin();
	bool hit{ false };
	Rectf hitBox{ player.GetHitBox() };
	while (it != m_EnemyProjectiles.end())
	{
		bool destroy{ (*it)->Update(elapsedSec, map) };
		if (destroy)
		{
			delete* it;
			it = m_EnemyProjectiles.erase(it);
		}
		else
		{
			if (!hit)
			{
				hit = (*it)->Hit(hitBox);
				if (hit)
				{
					player.Hit((*it)->GetDirection(), (*it)->GetDamage(), projectileManager);
				}
				++it;
			}
			else
			{
				++it;
			}
		}
	}
}

void ProjectileManager::DrawGround() const
{
	for (const Projectile* const projectile : m_PlayerProjectiles)
	{
		if (projectile->IsGround())
		{
			projectile->Draw();
		}
	}
}

void ProjectileManager::DrawUp() const
{
	for (const Projectile* const projectile : m_PlayerProjectiles)
	{
		if (!projectile->IsGround())
		{
			projectile->Draw();
		}
	}
}

void ProjectileManager::AddPlayerProjectile(Projectile* projectile)
{
	m_PlayerProjectiles.push_back(projectile);
}

void ProjectileManager::AddEnemyProjectile(Projectile* projectile)
{
	m_EnemyProjectiles.push_back(projectile);
}

Projectile* ProjectileManager::HitEnemy(const Rectf& enemyHitBox, const bool ground)
{
	auto it{ m_PlayerProjectiles.begin() };
	while (it != m_PlayerProjectiles.end())
	{
		if ((*it)->IsGround() == ground)
		{
			if ((*it)->Hit(enemyHitBox))
			{
				return *it;
			}
		}
		++it;
	}
	return nullptr;
}

void ProjectileManager::RemoveProjectiles()
{
	auto it{ m_PlayerProjectiles.begin() };
	for (; it != m_PlayerProjectiles.end();)
	{
		Interactable* interactable = dynamic_cast<Interactable*>(*it);
		if (!interactable)
		{
			delete* it;
		}
		it = m_PlayerProjectiles.erase(it);
	}
	it = m_EnemyProjectiles.begin();
	for (; it != m_EnemyProjectiles.end();)
	{
		delete* it;
		it = m_EnemyProjectiles.erase(it);
	}
}
