#pragma once
#include "Enemy.h"
#include <vector>
#include "Direction.h"
#include "StaticEnemy.h"

class SwordSoldier final : public Enemy, public StaticEnemy<SwordSoldier>
{
public:
	static void SetWeaponOffsets(const std::vector<Point2f>& weaponOffsets);
	static void SetWeaponSize(const float length, const float width);

	enum class Color
	{
		green = 0,
		blue = 1,
		red = 2
	};
	SwordSoldier(const Rectf& walkBox, Map* map, const Color color, const bool ground);
	~SwordSoldier() = default;

	SwordSoldier(const SwordSoldier& other) = delete;
	SwordSoldier(SwordSoldier&& other) noexcept = delete;
	SwordSoldier& operator=(const SwordSoldier& other) = delete;
	SwordSoldier& operator=(SwordSoldier&& other) noexcept = delete;

	void Draw() const override;

protected:
	void UpdateState(float& elapsedSec) override;
	void SetRandomDirection() override;
	void SetVelocity() override;
	void UpdateWaiting(const Player& player) override;
	void UpdateMoving(const float elapsedSec, const Player& player) override;
	void HitPlayer(Player& player, ProjectileManager& projectileManager) override;
	void WaitingAnimation() override;
	void MovingAnimation() override;
	void DamageAnimation() override;
	void DyingAnimation() override;
	void UpdateFrame(float frameTime) override;
	
	void GetHit(Direction direction) override;

	Rectf GetStaticHitBox() const override;
private:
	static const int m_NrOfColors;
	static const int m_HealthOffset;
	static const float m_PlayerDistance;
	static const float m_DetectSize;
	static const int m_DamageToPlayer;

	static std::vector<Point2f> m_WeaponOffsets;
	static float m_WeaponLength;
	static float m_WeaponWidth;

	Color m_Color;

	int InitialHealth(const Color color) const;
	Direction GetDirectionOffset(int offset) const;
	void FollowPlayer(const Player& player);
	void SeesPlayer(Direction direction, const Player& player);
	void LookSouth(const Player& player);
	void LookWest(const Player& player);
	void LookNorth(const Player& player);
	void LookEast(const Player& player);
	void FollowPlayer(const Direction direction);
	bool CheckPlayerDistance(const Player& player);
};

