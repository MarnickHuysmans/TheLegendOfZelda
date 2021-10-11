#include "pch.h"
#include "Floor.h"

Floor::Floor(const int id)
	: m_Id(id)
	, m_Level(nullptr)
{
}

Floor::~Floor()
{
	for (Map* map : m_Maps)
	{
		delete map;
	}
}

void Floor::Update(float elapsedSec, Player& player, ProjectileManager& projectileManager)
{
	m_EnemyManager.Update(elapsedSec, player, projectileManager);
}

void Floor::DrawGround(const Rectf& cameraTransform) const
{
	m_EnemyManager.DrawGround(cameraTransform);
}

void Floor::DrawUp(const Rectf& cameraTransform) const
{
	m_EnemyManager.DrawUp(cameraTransform);
}

void Floor::AddMap(Map* const map)
{
	map->SetFloor(this);
	m_Maps.push_back(map);
}

void Floor::AddEnemy(Enemy* const enemy)
{
	m_EnemyManager.AddEnemy(enemy);
}

Map* Floor::GetMap(int index)
{
	if (index >= 0 && index < m_Maps.size())
	{
		return m_Maps[index];
	}
	return nullptr;
}

Map* Floor::GetMapById(int id)
{
	if (id >= 0)
	{
		for (Map* map : m_Maps)
		{
			if (map->GetId() == id)
			{
				return map;
			}
		}
	}
	return nullptr;
}

void Floor::SetLevel(Level* level)
{
	m_Level = level;
}

void Floor::SetCurrentFloor()
{

}

Level* Floor::GetLevel() const
{
	return m_Level;
}

int Floor::GetId() const
{
	return m_Id;
}

void Floor::ResetPositions()
{
	m_EnemyManager.ResetPositions();
}

void Floor::Reset()
{
	m_EnemyManager.Reset();
	for (Map* map : m_Maps)
	{
		map->Reset();
	}
}
