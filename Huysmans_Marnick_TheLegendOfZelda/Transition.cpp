#include "pch.h"
#include "Transition.h"
#include "Map.h"
#include "Floor.h"
#include "Level.h"
#include "utils.h"
#include "Camera.h"
#include "SoundManager.h"
#include "ProjectileManager.h"
#include "PickUpManager.h"

Transition::Transition(Player& player)
	: m_WarpPoint(nullptr)
	, m_AnimTime(0.0f)
	, m_Player(player)
	, m_Transitioned(false)
{
}

bool Transition::Update(float elapsedSec, Level*& level, Camera& camera, SoundManager& soundManager, ProjectileManager& projectileManager, PickUpManager& pickUpManager)
{
	if (!m_WarpPoint)
	{
		return false;
	}
	if (!m_Transitioned)
	{
		m_AnimTime += elapsedSec;
		if (m_AnimTime >= m_TransitionTime)
		{
			m_Player.Warp(m_WarpPoint);
			Floor* floor = m_WarpPoint->GetMap()->GetFloor();
			Level* previousLevel{ level };
			level = floor->GetLevel();
			level->SetCurrentFloor(floor);
			if (level != previousLevel)
			{
				level->Reset();
			}
			m_Transitioned = true;
			m_AnimTime = m_TransitionTime;
			camera.CenterOn(m_Player.GetHitBox(), m_WarpPoint->GetMap());
			soundManager.StopSoundEffects();
			projectileManager.RemoveProjectiles();
			pickUpManager.RemovePickUps();
		}
	}
	else
	{
		m_AnimTime -= elapsedSec;
		if (m_AnimTime <= 0.0f)
		{
			m_WarpPoint = nullptr;
			m_Transitioned = false;
			m_AnimTime = 0.0f;
		}
	}
	return m_WarpPoint;
}

void Transition::Draw(const float width, const float height) const
{
	if (!m_WarpPoint)
	{
		return;
	}
	float transparency{ m_AnimTime / m_TransitionTime };
	utils::SetColor(Color4f{ 0.0f, 0.0f, 0.0f, transparency });
	utils::FillRect(0.0f, 0.0f, width, height);
}

void Transition::SetWarpPoint(WarpPoint* warpPoint)
{
	m_WarpPoint = warpPoint;
	m_Transitioned = false;
	m_AnimTime = 0.0f;
}
