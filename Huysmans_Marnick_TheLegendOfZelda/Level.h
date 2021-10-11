#pragma once
#include "EnemyManager.h"
#include "Floor.h"
#include "SoundStream.h"

class Level final
{
public:
	Level(const int id, SoundStream* start, SoundStream* loop);
	~Level();

	Level(const Level& other) = delete;
	Level(Level&& other) noexcept = delete;
	Level& operator=(const Level& other) = delete;
	Level& operator=(Level&& other) noexcept = delete;

	void Update(float elapsedSec, Player& player, ProjectileManager& projectileManager);
	void DrawGround(const Rectf& cameraTransform) const;
	void DrawUp(const Rectf& cameraTransform) const;
	void AddFloor(Floor* const floor);
	Floor* GetFloor(int index) const;
	Floor* GetCurrentFloor() const;
	void SetCurrentFloor(Floor* floor);
	int GetId() const;
	void Reset();
	bool IsFree() const;
	void PlayMusic() const;
	void PlayLoop() const;
private:
	const bool m_Free;
	int m_Id;
	std::vector<Floor*> m_Floors;
	int m_CurrentFloor;
	SoundStream* m_Start;
	SoundStream* m_Loop;
};

