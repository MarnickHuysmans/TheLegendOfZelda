#pragma once
#include <vector>
#include "Enemy.h"

class EnemyManager final
{
public:
	EnemyManager();
	~EnemyManager();

	EnemyManager(const EnemyManager& other) = delete;
	EnemyManager(EnemyManager&& other) noexcept = delete;
	EnemyManager& operator=(const EnemyManager& other) = delete;
	EnemyManager& operator=(EnemyManager&& other) noexcept = delete;
	
	void Update(float elapsedSec, Player& player, ProjectileManager& projectileManager);
	void DrawGround(const Rectf& cameraTransform) const;
	void DrawUp(const Rectf& cameraTransform) const;
	void AddEnemy(Enemy* enemy);
	void ResetPositions();
	void Reset();
private:
	std::vector<Enemy*> m_Enemies;
	Point2f m_CameraOffset;
	Point2f m_PlayerOffset;

	Rectf DrawArea(const Rectf& cameraTransform) const;
};

