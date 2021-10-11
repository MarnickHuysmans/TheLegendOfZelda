#pragma once
#include "Map.h"
#include "EnemyManager.h"

class Floor final
{
public:
	Floor(const int id);
	~Floor();

	Floor(const Floor& other) = delete;
	Floor(Floor&& other) noexcept = delete;
	Floor& operator=(const Floor& other) = delete;
	Floor& operator=(Floor&& other) noexcept = delete;

	void Update(float elapsedSec, Player& player, ProjectileManager& projectileManager);
	void DrawGround(const Rectf& cameraTransform) const;
	void DrawUp(const Rectf& cameraTransform) const;
	void AddMap(Map* const map);
	void AddEnemy(Enemy* const enemy);
	Map* GetMap(int index);
	Map* GetMapById(int id);
	void SetLevel(Level* level);
	void SetCurrentFloor();
	Level* GetLevel() const;
	int GetId() const;
	void ResetPositions();
	void Reset();
private:
	int m_Id;
	EnemyManager m_EnemyManager;
	std::vector<Map*> m_Maps;
	Level* m_Level;
};

