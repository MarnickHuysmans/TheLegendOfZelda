#pragma once
#include "Enemy.h"
#include "StaticEnemy.h"

class Rat final : public Enemy, public StaticEnemy<Rat>
{
public:
	Rat(const Rectf& walkBox, Map* map, bool ground, int health = 1);
	~Rat() = default;

	Rat(const Rat& other) = delete;
	Rat(Rat&& other) noexcept = delete;
	Rat& operator=(const Rat& other) = delete;
	Rat& operator=(Rat&& other) noexcept = delete;

	void Draw() const override;

protected:
	void UpdateState(float& elapsedSec) override;
	void SetRandomDirection() override;
	void UpdateWaiting(const Player& player) override;
	void UpdateMoving(const float elapsedSec, const Player& player) override;
	void WaitingAnimation() override;
	void MovingAnimation() override;
	void DamageAnimation() override;
	void DyingAnimation() override;
	void UpdateFrame(float frameTime) override;

	void GetHit(Direction direction) override;

	Rectf GetStaticHitBox() const override;
};

