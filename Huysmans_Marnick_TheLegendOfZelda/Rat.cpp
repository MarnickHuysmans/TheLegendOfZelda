#include "pch.h"
#include "Rat.h"
#include "Texture.h"
#include "utils.h"

Rat::Rat(const Rectf& walkBox, Map* map, bool ground, int health)
	: Enemy(walkBox, map, health, ground, 50.0f, 125.0f)
{
}

void Rat::Draw() const
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
	float height{ sprite->GetHeight() / static_cast<float>(m_NrOfDirections) };

	float left{ width * static_cast<float>(m_AnimFrame) };
	float bottom{ height * static_cast<float>(direction + 1) };

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

void Rat::UpdateState(float& elapsedSec)
{
	if (m_ActionState == ActionState::dying)
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

void Rat::SetRandomDirection()
{
	Direction randomDirection{ static_cast<Direction>(rand() % m_NrOfDirections) };
	m_Direction = randomDirection;
	SetVelocity();
	if (m_EnemySound)
	{
		m_EnemySound->Play(0);
	}
}

void Rat::UpdateWaiting(const Player& player)
{
}

void Rat::UpdateMoving(const float elapsedSec, const Player& player)
{
	if (m_Bump)
	{
		BumpMoving();
	}
	Move(elapsedSec);
}

void Rat::WaitingAnimation()
{
	float frameTime{ 1.0f / m_FramesPerSecond[static_cast<int>(ActionState::waiting)] };
	UpdateFrame(frameTime);
}

void Rat::MovingAnimation()
{
	float frameTime{ 1.0f / m_FramesPerSecond[static_cast<int>(ActionState::moving)] };
	UpdateFrame(frameTime);
}

void Rat::DamageAnimation()
{
	float frameTime{ 1.0f / m_FramesPerSecond[static_cast<int>(ActionState::damage)] };
	UpdateFrame(frameTime);
}

void Rat::DyingAnimation()
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

void Rat::UpdateFrame(float frameTime)
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

void Rat::GetHit(Direction direction)
{
	if (m_HitSound)
	{
		m_HitSound->Play(0);
	}
	Enemy::GetHit(direction);
}

Rectf Rat::GetStaticHitBox() const
{
	return StaticEnemy<Rat>::GetStaticHitBox();
}
