#include "pch.h"
#include "Enemy.h"
#include "utils.h"
#include "Projectile.h"
#include "ProjectileManager.h"
#include "Player.h"
#include "Map.h"

const int Enemy::m_NrOfDirections{ 4 };
SoundEffect* Enemy::m_DeathSound{ nullptr };

void Enemy::SetDeathSound(SoundEffect* soundEffect)
{
	m_DeathSound = soundEffect;
}

Enemy::Enemy(const Rectf& walkBox, Map* map, const int health, const bool ground, const float walkSpeed, const float damageSpeed)
	: m_Start(walkBox)
	, m_WalkBox(walkBox)
	, m_Ground(ground)
	, m_Map(map)
	, m_Health(health)
	, m_Direction(Direction::south)
	, m_ActionState(ActionState::waiting)
	, m_Player(false)
	, m_StateTime(0.0f)
	, m_AnimTime(0.0f)
	, m_AnimFrame(0)
	, m_Bump(false)
	, m_Dead(false)
	, m_Damage(false)
	, m_PlayerOverlapDamage(1)
	, m_WalkSpeed(walkSpeed)
	, m_DamageSpeed(damageSpeed)
{
}

void Enemy::Update(float elapsedSec, Player& player, ProjectileManager& projectileManager)
{
	if (m_Dead)
	{
		return;
	}
	UpdateState(elapsedSec);

	UpdateAnimation(elapsedSec);

	UpdateCurrentState(elapsedSec, player);

	if (PlayerHit(player))
	{
		return;
	}

	if (ProjectileHit(projectileManager))
	{
		return;
	}

	HitPlayer(player, projectileManager);
}

void Enemy::DrawGround() const
{
	if (m_Ground)
	{
		Draw();
	}
}

void Enemy::DrawUp() const
{
	if (!m_Ground)
	{
		Draw();
	}
}

Rectf Enemy::GetWalkBox() const
{
	return m_WalkBox;
}

Rectf Enemy::GetHitBox() const
{
	Rectf staticHitBox{ GetStaticHitBox() };
	staticHitBox.left += m_WalkBox.left;
	staticHitBox.bottom += m_WalkBox.bottom;
	return staticHitBox;
}

void Enemy::ResetPosition()
{
	m_WalkBox = m_Start;
}

void Enemy::Reset()
{
	m_ActionState = ActionState::waiting;
	m_Direction = Direction::south;
	m_Dead = false;
	m_Damage = false;
	ResetState();
	ResetAnimation();
}

void Enemy::SwitchState()
{
	switch (m_ActionState)
	{
	case ActionState::waiting:
		SwitchToMoving();
		break;
	case ActionState::moving:
		if (!m_Player)
		{
			SwitchToWaiting();
		}
		break;
	case ActionState::damage:
		m_Player = true;
		SwitchToMoving();
		break;
	}
}

void Enemy::SwitchToWaiting()
{
	m_ActionState = ActionState::waiting;
	m_Velocity = Vector2f{ 0.0f, 0.0f };
	ResetAnimation();
}

void Enemy::SwitchToMoving()
{
	m_ActionState = ActionState::moving;
	SetRandomDirection();
	ResetAnimation();
}

void Enemy::SwitchToDamage()
{
	m_ActionState = ActionState::damage;
	ResetState();
	ResetAnimation();
}

void Enemy::SetVelocity()
{
	switch (m_Direction)
	{
	case Direction::south:
		m_Velocity.x = 0.0f;
		m_Velocity.y = -m_WalkSpeed;
		break;
	case Direction::west:
		m_Velocity.x = -m_WalkSpeed;
		m_Velocity.y = 0.0f;
		break;
	case Direction::north:
		m_Velocity.x = 0.0f;
		m_Velocity.y = m_WalkSpeed;
		break;
	case Direction::east:
		m_Velocity.x = m_WalkSpeed;
		m_Velocity.y = 0.0f;
		break;
	}
}

void Enemy::UpdateCurrentState(const float elapsedSec, const Player& player)
{
	switch (m_ActionState)
	{
	case ActionState::waiting:
		UpdateWaiting(player);
		break;
	case ActionState::moving:
		UpdateMoving(elapsedSec, player);
		break;
	case ActionState::damage:
		UpdateDamage(elapsedSec);
		break;
	case ActionState::dying:
		break;
	}
}

void Enemy::UpdateDamage(const float elapsedSec)
{
	Move(elapsedSec);
}

void Enemy::Move(const float elapsedSec)
{
	m_WalkBox.left += m_Velocity.x * elapsedSec;
	m_WalkBox.bottom += m_Velocity.y * elapsedSec;
	m_Bump = m_Map->HandleCollision(m_WalkBox, m_Velocity, m_Ground);
	CheckStairs();
	m_Map = m_Map->MapChange(m_WalkBox);
}

void Enemy::CheckStairs()
{
	Stairs* stairs{ m_Map->CheckStairs(m_WalkBox) };
	if (stairs)
	{
		m_Ground = stairs->IsGround();
	}
}

void Enemy::BumpMoving()
{
	m_Bump = false;
	ResetAnimation();
	m_Direction = OppositeDirection(m_Direction);
	SetVelocity();
}


bool Enemy::PlayerHit(Player& player)
{
	if (m_ActionState == ActionState::damage || m_ActionState == ActionState::dying)
	{
		return true;
	}
	std::vector<Point2f> sword{};
	if (player.SwordHitBox(sword))
	{
		if (utils::IsOverlapping(sword, GetHitBox()))
		{
			GetHitDamage(player.GetDirection(), player.GetSwordDamage());
			return true;
		}
	}
	return false;
}

bool Enemy::ProjectileHit(ProjectileManager& projectileManager)
{
	Rectf hitBox{ GetHitBox() };
	Projectile* projectile{ projectileManager.HitEnemy(hitBox, m_Ground) };
	if (projectile)
	{
		GetHitDamage(projectile->GetDirection(), projectile->GetDamage());
		return true;
	}
	return false;
}

void Enemy::HitPlayer(Player& player, ProjectileManager& projectileManager)
{
	if (m_Ground != player.IsGround())
	{
		return;
	}
	if (m_ActionState != ActionState::damage)
	{
		if (utils::IsOverlapping(player.GetHitBox(), GetHitBox()))
		{
			player.Hit(OppositeDirection(player.GetDirection()), m_PlayerOverlapDamage, projectileManager);
		}
	}
}

void Enemy::GetHitDamage(Direction direction, int damage)
{
	m_Health -= damage;
	if (m_Health > 0)
	{
		GetHit(direction);
	}
	else
	{
		Dying();
	}
}

void Enemy::GetHit(Direction direction)
{
	SwitchToDamage();
	m_Player = false;
	m_Damage = true;
	m_Direction = direction;
	SetVelocity();
	switch (direction)
	{
	case Direction::south:
		m_Direction = Direction::north;
		break;
	case Direction::west:
		m_Direction = Direction::east;
		break;
	case Direction::north:
		m_Direction = Direction::south;
		break;
	case Direction::east:
		m_Direction = Direction::west;
		break;
	}
}

void Enemy::Dying()
{
	m_ActionState = ActionState::dying;
	if (m_DeathSound)
	{
		m_DeathSound->Play(0);
	}
}

void Enemy::UpdateAnimation(const float elapsedSec)
{
	m_AnimTime += elapsedSec;
	switch (m_ActionState)
	{
	case ActionState::waiting:
		WaitingAnimation();
		break;
	case ActionState::moving:
		MovingAnimation();
		break;
	case ActionState::damage:
		DamageAnimation();
		break;
	case ActionState::dying:
		DyingAnimation();
		break;
	}
}

void Enemy::ResetState()
{
	m_StateTime = 0.0f;
}

void Enemy::ResetAnimation()
{
	m_AnimTime = 0.0f;
	m_AnimFrame = 0;
}

Direction Enemy::OppositeDirection(Direction direction)
{
	switch (direction)
	{
	case Direction::south:
		return Direction::north;
	case Direction::west:
		return Direction::east;
	case Direction::north:
		return Direction::south;
	case Direction::east:
		return Direction::west;
	}
	return Direction::south;
}
