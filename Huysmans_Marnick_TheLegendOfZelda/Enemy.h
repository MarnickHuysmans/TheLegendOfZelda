#pragma once
#include "Direction.h"
#include "Vector2f.h"
#include "SoundEffect.h"

class Transition;
class ProjectileManager;
class Player;
class Map;

class Enemy
{
public:
	enum class ActionState
	{
		waiting = 0,
		moving = 1,
		damage = 2,
		dying = 3

	};

	static void SetDeathSound(SoundEffect* soundEffect);

	Enemy(const Rectf& walkBox, Map* map, const int health, const bool ground, const float walkSpeed, const float damageSpeed);
	virtual ~Enemy() {}

	virtual void Update(float elapsedSec, Player& player, ProjectileManager& projectileManager);
	virtual void DrawGround() const;
	virtual void DrawUp() const;
	virtual void Draw() const = 0;
	virtual Rectf GetWalkBox() const;
	virtual Rectf GetHitBox() const;
	virtual void ResetPosition();
	virtual void Reset();
protected:
	static const int m_NrOfDirections;
	static SoundEffect* m_DeathSound;

	Rectf m_Start;
	Rectf m_WalkBox;
	bool m_Ground;
	Map* m_Map;
	int m_Health;
	Vector2f m_Velocity;
	Direction m_Direction;
	ActionState m_ActionState;
	bool m_Player;
	float m_StateTime;
	float m_AnimTime;
	int m_AnimFrame;
	bool m_Bump;
	bool m_Dead;
	bool m_Damage;
	int m_PlayerOverlapDamage;

	float m_WalkSpeed;
	float m_DamageSpeed;

	virtual void UpdateState(float& elapsedSec) = 0;
	virtual void SwitchState();
	virtual void SwitchToWaiting();
	virtual void SwitchToMoving();
	virtual void SwitchToDamage();
	virtual void SetRandomDirection() = 0;
	virtual void SetVelocity();
	virtual void UpdateCurrentState(const float elapsedSec, const Player& player);
	virtual void UpdateWaiting(const Player& player) = 0;
	virtual void UpdateMoving(const float elapsedSec, const Player& player) = 0;
	virtual void UpdateDamage(const float elapsedSec);
	virtual void Move(const float elapsedSec);
	virtual void CheckStairs();
	virtual void BumpMoving();
	virtual bool PlayerHit(Player& player);
	virtual bool ProjectileHit(ProjectileManager& projectileManager);
	virtual void HitPlayer(Player& player, ProjectileManager& projectileManager);
	virtual void GetHitDamage(Direction direction, int damage);
	virtual void GetHit(Direction direction);
	virtual void Dying();
	virtual void UpdateAnimation(const float elapsedSec);
	virtual void WaitingAnimation() = 0;
	virtual void MovingAnimation() = 0;
	virtual void DamageAnimation() = 0;
	virtual void DyingAnimation() = 0;
	virtual void UpdateFrame(float frameTime) = 0;
	virtual void ResetState();
	virtual void ResetAnimation();
	Direction OppositeDirection(Direction direction);
	virtual Rectf GetStaticHitBox() const = 0;
};

