#include "pch.h"
#include "Level.h"
#include "Map.h"
#include "Floor.h"

Level::Level(const int id, SoundStream* start, SoundStream* loop)
	: m_Free(true)
	, m_Id(id)
	, m_CurrentFloor(0)
	, m_Start(start)
	, m_Loop(loop)
{
}

Level::~Level()
{
	for (Floor* floor : m_Floors)
	{
		delete floor;
	}
}

void Level::Update(float elapsedSec, Player& player, ProjectileManager& projectileManager)
{
	PlayLoop();
	m_Floors[m_CurrentFloor]->Update(elapsedSec, player, projectileManager);
}

void Level::DrawGround(const Rectf& cameraTransform) const
{
	m_Floors[m_CurrentFloor]->DrawGround(cameraTransform);
}

void Level::DrawUp(const Rectf& cameraTransform) const
{
	m_Floors[m_CurrentFloor]->DrawUp(cameraTransform);
}

void Level::AddFloor(Floor* const floor)
{
	floor->SetLevel(this);
	m_Floors.push_back(floor);
}

Floor* Level::GetFloor(int index) const
{
	return m_Floors[index];
}

Floor* Level::GetCurrentFloor() const
{
	return m_Floors[m_CurrentFloor];
}

void Level::SetCurrentFloor(Floor* floor)
{
	auto it = std::find(m_Floors.begin(), m_Floors.end(), floor);
	if (it != m_Floors.end())
	{
		m_CurrentFloor = static_cast<int>(it - m_Floors.begin());
		GetCurrentFloor()->ResetPositions();
	}
}

int Level::GetId() const
{
	return m_Id;
}

void Level::Reset()
{
	PlayMusic();
	for (Floor* floor : m_Floors)
	{
		floor->Reset();
	}
}

bool Level::IsFree() const
{
	return m_Free;
}

void Level::PlayMusic() const
{
	m_Start->Play(false);
}

void Level::PlayLoop() const
{
	if (!SoundStream::IsPlaying())
	{
		m_Loop->Play(true);
	}
}
