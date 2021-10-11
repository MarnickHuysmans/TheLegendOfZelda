#include "pch.h"
#include "SwordSoldier.h"
#include "utils.h"
#include "Player.h"
#include "Map.h"

const int SwordSoldier::m_NrOfColors{ 3 };

const int SwordSoldier::m_HealthOffset{ 2 };
const float SwordSoldier::m_PlayerDistance{ 100.0f };
const float SwordSoldier::m_DetectSize{ 50.0f };
const int SwordSoldier::m_DamageToPlayer{ 1 };

std::vector<Point2f> SwordSoldier::m_WeaponOffsets{};
float SwordSoldier::m_WeaponLength{};
float SwordSoldier::m_WeaponWidth{};

void SwordSoldier::SetWeaponOffsets(const std::vector<Point2f>& weaponOffsets)
{
	m_WeaponOffsets = weaponOffsets;
}

void SwordSoldier::SetWeaponSize(const float length, const float width)
{
	m_WeaponLength = length;
	m_WeaponWidth = width;
}

SwordSoldier::SwordSoldier(const Rectf& walkBox, Map* map, const Color color, const bool ground)
	: Enemy(walkBox, map, InitialHealth(color), ground, 40.0f, 125.0f)
	, m_Color(color)
{
}

void SwordSoldier::Draw() const
{
	if (m_Dead)
	{
		return;
	}
	int actionState{ static_cast<int>(m_ActionState) };
	int direction{ static_cast<int>(m_Direction) };


	const Texture* sprite = m_SpriteTextures[actionState];
	int nrOfFrames = m_NrOfFrames[actionState][m_NrOfDirections];
	Point2f offset = m_SpriteOffsets[actionState];

	float width{ sprite->GetWidth() / static_cast<float>(nrOfFrames) };
	float height{ };

	float left{ width * static_cast<float>(m_AnimFrame) };
	float bottom{ };

	if (m_ActionState != ActionState::dying)
	{
		int color{ static_cast<int>(m_Color) };
		float colorOffset{ sprite->GetHeight() / static_cast<float>(m_NrOfColors) };
		height = colorOffset / static_cast<float>(m_NrOfDirections);
		bottom = height * static_cast<float>(direction + 1) + colorOffset * static_cast<float>(color);
	}
	else
	{
		height = sprite->GetHeight() / static_cast<float>(m_NrOfDirections);
		bottom = height * static_cast<float>(direction + 1);
	}

	Point2f bottomLeft{ m_WalkBox.left - offset.x, m_WalkBox.bottom - offset.y };
	Rectf srcRect{ left, bottom, width, height };

	sprite->Draw(bottomLeft, srcRect);

#if defined(DEBUG) | defined(_DEBUG)
	utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
	utils::DrawRect(m_WalkBox);

	utils::SetColor(Color4f{ 1.0f, 0.0f,0.0f,1.0f });
	utils::DrawRect(GetHitBox());
#endif
}

int SwordSoldier::InitialHealth(const Color color) const
{
	return static_cast<int>(color) + m_HealthOffset;
}

void SwordSoldier::UpdateState(float& elapsedSec)
{
	if (m_Player || m_ActionState == ActionState::dying)
	{
		return;
	}
	int actionState{ static_cast<int>(m_ActionState) };
	float time{ m_StateTimings[actionState] };
	m_StateTime += elapsedSec;
	if (m_StateTime >= time)
	{
		m_StateTime -= time;
		elapsedSec -= m_StateTime;
		m_StateTime = 0.0f;
		SwitchState();
	}
}

void SwordSoldier::SetRandomDirection()
{
	int offset{ rand() % 3 - 1 };
	m_Direction = GetDirectionOffset(offset);
	SetVelocity();
}

void SwordSoldier::SetVelocity()
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

Direction SwordSoldier::GetDirectionOffset(int offset) const
{
	int direction{ static_cast<int>(m_Direction) };
	direction += offset;
	if (direction < 0)
	{
		direction = m_NrOfDirections - 1;
	}
	else if (direction >= m_NrOfDirections)
	{
		direction = 0;
	}
	return static_cast<Direction>(direction);
}

void SwordSoldier::UpdateWaiting(const Player& player)
{
	int directionOffset{};
	switch (m_AnimFrame)
	{
	case 1:
		directionOffset = -1;
		break;
	case 0:
		directionOffset = 0;
		break;
	case 2:
		directionOffset = 1;
		break;
	}
	Direction direction{ GetDirectionOffset(directionOffset) };
	SeesPlayer(direction, player);
}

void SwordSoldier::UpdateMoving(const float elapsedSec, const Player& player)
{
	if (m_Player)
	{
		FollowPlayer(player);
	}
	else if (m_Bump)
	{
		BumpMoving();
	}
	Move(elapsedSec);
}

void SwordSoldier::FollowPlayer(const Player& player)
{
	if (CheckPlayerDistance(player))
	{
		return;
	}
	Rectf playerRect{ player.GetWalkBox() };
	float x{ m_WalkBox.left - playerRect.left };
	float y{ m_WalkBox.bottom - playerRect.bottom };
	if (x > 2.0f)
	{
		m_Velocity.x = -m_WalkSpeed;
	}
	else if (x < -2.0f)
	{
		m_Velocity.x = m_WalkSpeed;
	}
	else
	{
		m_Velocity.x = 0.0f;
	}
	if (y > 2.0f)
	{
		m_Velocity.y = -m_WalkSpeed;
	}
	else if (y < -2.0f)
	{
		m_Velocity.y = m_WalkSpeed;
	}
	else
	{
		m_Velocity.y = 0.0f;
	}
	if (abs(x) > abs(y))
	{
		if (x > 0.0f)
		{
			m_Direction = Direction::west;
		}
		else
		{
			m_Direction = Direction::east;
		}
	}
	else if (abs(x) < abs(y))
	{
		if (y > 0.0f)
		{
			m_Direction = Direction::south;
		}
		else
		{
			m_Direction = Direction::north;
		}
	}
}

void SwordSoldier::SeesPlayer(Direction direction, const Player& player)
{
	switch (direction)
	{
	case Direction::south:
		LookSouth(player);
		break;
	case Direction::west:
		LookWest(player);
		break;
	case Direction::north:
		LookNorth(player);
		break;
	case Direction::east:
		LookEast(player);
		break;
	}
}

void SwordSoldier::LookSouth(const Player& player)
{
	float centerX{ m_WalkBox.left + m_WalkBox.width / 2.0f };
	float left{ centerX - m_DetectSize / 2.0f };
	float bottom{ m_WalkBox.bottom - m_DetectSize };
	Rectf look{ left, bottom, m_DetectSize, m_DetectSize };
	if (utils::IsOverlapping(look, player.GetWalkBox()))
	{
		FollowPlayer(Direction::south);
	}
}

void SwordSoldier::LookWest(const Player& player)
{
	float centerY{ m_WalkBox.bottom + m_WalkBox.height / 2.0f };
	float left{ m_WalkBox.left - m_DetectSize };
	float bottom{ centerY - m_DetectSize / 2.0f };
	Rectf look{ left, bottom, m_DetectSize, m_DetectSize };
	if (utils::IsOverlapping(look, player.GetWalkBox()))
	{
		FollowPlayer(Direction::west);
	}
}

void SwordSoldier::LookNorth(const Player& player)
{
	float centerX{ m_WalkBox.left + m_WalkBox.width / 2.0f };
	float left{ centerX - m_DetectSize / 2.0f };
	float bottom{ m_WalkBox.bottom + m_WalkBox.height + m_DetectSize };
	Rectf look{ left, bottom, m_DetectSize, m_DetectSize };
	if (utils::IsOverlapping(look, player.GetWalkBox()))
	{
		FollowPlayer(Direction::north);
	}
}

void SwordSoldier::LookEast(const Player& player)
{
	float centerY{ m_WalkBox.bottom + m_WalkBox.height / 2.0f };
	float left{ m_WalkBox.left + m_WalkBox.width };
	float bottom{ centerY - m_DetectSize / 2.0f };
	Rectf look{ left, bottom, m_DetectSize, m_DetectSize };
	if (utils::IsOverlapping(look, player.GetWalkBox()))
	{
		FollowPlayer(Direction::east);
	}
}

void SwordSoldier::FollowPlayer(const Direction direction)
{
	m_Player = true;
	m_ActionState = ActionState::moving;
	m_Direction = direction;
	ResetState();
	ResetAnimation();
	if (m_EnemySound)
	{
		m_EnemySound->Play(0);
	}
}

bool SwordSoldier::CheckPlayerDistance(const Player& player)
{
	Point2f bottomLeft{ m_WalkBox.left, m_WalkBox.bottom };
	Rectf playerRect{ player.GetWalkBox() };
	Point2f bottomLeftPlayer{ playerRect.left, playerRect.bottom };
	Vector2f distance{ bottomLeft, bottomLeftPlayer };
	if (distance.Length() > m_PlayerDistance)
	{
		m_Player = false;
		ResetState();
		SwitchToWaiting();
		return true;
	}
	return false;
}

void SwordSoldier::HitPlayer(Player& player, ProjectileManager& projectileManager)
{
	if (m_Ground != player.IsGround())
	{
		return;
	}
	int direction{ static_cast<int>(m_Direction) };
	int moving{ static_cast<int>(ActionState::moving) };
	if (m_ActionState == ActionState::moving && m_AnimFrame == m_NrOfFrames[moving][direction] - 1)
	{
		Point2f offset = m_WeaponOffsets[direction];
		Rectf weapon{};
		weapon.left = m_WalkBox.left + offset.x;
		weapon.bottom = m_WalkBox.bottom + offset.y;
		if (m_Direction == Direction::east || m_Direction == Direction::west)
		{
			weapon.width = m_WeaponLength;
			weapon.height = m_WeaponWidth;
		}
		else
		{
			weapon.width = m_WeaponWidth;
			weapon.height = m_WeaponLength;
		}
		if (utils::IsOverlapping(player.GetHitBox(), weapon))
		{
			player.Hit(m_Direction, m_DamageToPlayer, projectileManager);
			return;
		}
	}
	Enemy::HitPlayer(player, projectileManager);
}

void SwordSoldier::WaitingAnimation()
{
	int actionState{ static_cast<int>(ActionState::waiting) };
	int direction{ static_cast<int>(m_Direction) };
	float time{ m_StateTimings[actionState] };
	int frames{ m_NrOfFrames[actionState][direction] };
	float frameTime{ time / static_cast<float>(frames) };
	UpdateFrame(frameTime);
}

void SwordSoldier::MovingAnimation()
{
	float frameTime{ 1.0f / m_FramesPerSecond[static_cast<int>(ActionState::moving)] };
	UpdateFrame(frameTime);
}

void SwordSoldier::DamageAnimation()
{
	float frameTime{ 1.0f / m_FramesPerSecond[static_cast<int>(ActionState::damage)] };
	UpdateFrame(frameTime);
}

void SwordSoldier::DyingAnimation()
{
	int actionState{ static_cast<int>(ActionState::dying) };
	int direction{ static_cast<int>(m_Direction) };
	int lastFrames = m_NrOfFrames[actionState][direction] - 1;
	float frameTime{ 1.0f / m_FramesPerSecond[actionState] };
	if (m_AnimFrame == lastFrames && m_AnimTime >= frameTime)
	{
		m_Dead = true;
	}
	else
	{
		UpdateFrame(frameTime);
	}
}

void SwordSoldier::UpdateFrame(float frameTime)
{
	if (m_AnimTime >= frameTime)
	{
		int actionState{ static_cast<int>(m_ActionState) };
		int direction{ static_cast<int>(m_Direction) };
		int nrOfFrames = m_NrOfFrames[actionState][direction];
		m_AnimTime -= frameTime;
		++m_AnimFrame;
		m_AnimFrame = m_AnimFrame % nrOfFrames;
	}
}

void SwordSoldier::GetHit(Direction direction)
{
	if (m_HitSound)
	{
		m_HitSound->Play(0);
	}
	Enemy::GetHit(direction);
}

Rectf SwordSoldier::GetStaticHitBox() const
{
	return StaticEnemy<SwordSoldier>::GetStaticHitBox();
}
