#pragma once
#include "WarpPoint.h"
#include "Player.h"

class SoundManager;
class Camera;

class Transition final
{
public:
	Transition(Player& player);
	bool Update(float elapsedSec, Level*& level, Camera& camera, SoundManager& soundManager, ProjectileManager& projectileManager, PickUpManager& pickUpManager);
	void Draw(const float width, const float height) const;
	void SetWarpPoint(WarpPoint* warpPoint);
private:
	const float m_TransitionTime{ 0.5f };

	WarpPoint* m_WarpPoint;
	float m_AnimTime;
	Player& m_Player;
	bool m_Transitioned;
};

