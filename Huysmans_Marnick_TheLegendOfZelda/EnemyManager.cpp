#include "pch.h"
#include "EnemyManager.h"
#include "utils.h"
#include "Player.h"

EnemyManager::EnemyManager()
	: m_CameraOffset(100.0f, 100.0f)
	, m_PlayerOffset(300.0f, 300.0f)
{
}

EnemyManager::~EnemyManager()
{
	for (Enemy* enemy : m_Enemies)
	{
		delete enemy;
	}
}

void EnemyManager::Update(float elapsedSec, Player& player, ProjectileManager& projectileManager)
{
	Rectf updateArea{ player.GetWalkBox() };
	updateArea.left -= m_PlayerOffset.x;
	updateArea.bottom -= m_PlayerOffset.y;
	updateArea.width = m_PlayerOffset.x * 2.0f;
	updateArea.height = m_PlayerOffset.y * 2.0f;
	for (Enemy* enemy : m_Enemies)
	{
		if (utils::IsOverlapping(enemy->GetWalkBox(), updateArea))
		{
			enemy->Update(elapsedSec, player, projectileManager);
		}
	}
}

void EnemyManager::DrawGround(const Rectf& cameraTransform) const
{
	Rectf drawArea{ DrawArea(cameraTransform) };
	for (const Enemy* const enemy : m_Enemies)
	{
		if (utils::IsOverlapping(enemy->GetWalkBox(), drawArea))
		{
			enemy->DrawGround();
		}
	}
}

void EnemyManager::DrawUp(const Rectf& cameraTransform) const
{
	Rectf drawArea{ DrawArea(cameraTransform) };
	for (const Enemy* const enemy : m_Enemies)
	{
		if (utils::IsOverlapping(enemy->GetWalkBox(), drawArea))
		{
			enemy->DrawUp();
		}
	}
}

void EnemyManager::AddEnemy(Enemy* enemy)
{
	m_Enemies.push_back(std::move(enemy));
}

void EnemyManager::ResetPositions()
{
	for (Enemy* enemy : m_Enemies)
	{
		enemy->ResetPosition();
	}
}

void EnemyManager::Reset()
{
	for (Enemy* enemy : m_Enemies)
	{
		enemy->Reset();
	}
}

Rectf EnemyManager::DrawArea(const Rectf& cameraTransform) const
{
	Rectf drawArea{ cameraTransform.left - m_CameraOffset.x / 2.0f, cameraTransform.bottom - m_CameraOffset.y / 2.0f, cameraTransform.width + m_CameraOffset.x, cameraTransform.height + m_CameraOffset.y };
	return drawArea;
}
