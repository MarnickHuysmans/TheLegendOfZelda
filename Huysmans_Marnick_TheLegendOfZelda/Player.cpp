#include "pch.h"
#include "Player.h"
#include "Map.h"
#include "utils.h"
#include "ProjectileManager.h"
#include <iostream>
#include "SVGParser.h"
#include "Transition.h"
#include "SoundManager.h"
#include "TextureManager.h"

Player::Player(SoundManager& soundManager, TextureManager& textureManager)
	: m_MaxHealth(6)
	, m_Health(6)
	, m_WalkBox(122.0f, 109.0f, 12.0f, 6.0f)
	, m_HitBox(0.0f, 2.0f, 12.0f, 14.0f)
	, m_Ground(true)
	, m_Shield(true)
	, m_Direction(Direction::south)
	, m_ActionState(ActionState::waiting)
	, m_AnimTime(0.0f)
	, m_AnimFrame(0)
	, m_Hit(false)
	, m_Damage(false)
	, m_Draw(true)
	, m_HitTime(0.0f)
	, m_Rupees(0)
	, m_DyingFrames(0)
	, m_DyingFrame(0)
	, m_SoundManager(soundManager)
{
	InitializeNrOfFrames();
	InitializeAnimationTime();
	InitializeTextures(textureManager);
	InitializeSpriteOffset();
	InitializeSwordHitBoxes();
	InitializeSounds(soundManager);
}

Player::~Player()
{
	delete m_DeathWarp;
}

bool Player::Update(float elapsedSec, ProjectileManager& projectileManager, Transition& transition, PickUpManager& pickUpManager)
{
	if (m_ActionState == ActionState::dying)
	{
		UpdateDying(elapsedSec);
		return true;
	}
	if (m_ActionState == ActionState::death)
	{
		UpdateDeath(elapsedSec, transition);
		return true;
	}
	if (m_Hit)
	{
		if (UpdateHit(elapsedSec, projectileManager))
		{
			return Moving(elapsedSec, projectileManager, transition);
		}
	}

	if (m_ActionState == ActionState::sword)
	{
		if (UpdateFrame(elapsedSec))
		{
			HitInteractables(projectileManager, pickUpManager);
			return false;
		}
	}
	if (m_Interactable)
	{
		if (m_ActionState == ActionState::pickUp)
		{
			if (UpdateFrame(elapsedSec))
			{
				PickUp();
				return false;
			}
			PickUp();
		}
	}
	if (m_ActionState == ActionState::throwing)
	{
		if (UpdateFrame(elapsedSec))
		{
			return false;
		}
	}


	Direction oldDirection{ m_Direction };
	ActionState oldActionState{ m_ActionState };

	m_Velocity = Vector2f{ 0.0f, 0.0f };

	ProcessKeyBoardState(pickUpManager);

	if (oldDirection != m_Direction || oldActionState != m_ActionState)
	{
		ResetAnimation();
	}

	if (m_ActionState != ActionState::waiting && m_ActionState != ActionState::waitingPickUp)
	{
		UpdateFrame(elapsedSec);

		if (m_ActionState == ActionState::moving || m_ActionState == ActionState::movingPickUp)
		{
			return Moving(elapsedSec, projectileManager, transition);
		}
		if (m_ActionState == ActionState::pickUp)
		{
			PickUp();
			return false;
		}
		if (m_ActionState == ActionState::throwing)
		{
			Throw(projectileManager);
			return false;
		}
	}
	return false;
}

void Player::DrawGround() const
{
	if (m_Ground)
	{
		Draw();
	}
}

void Player::DrawUp() const
{
	if (!m_Ground)
	{
		Draw();
	}
}

bool Player::Moving(float elapsedSec, ProjectileManager& projectileManager, Transition& transition)
{
	m_WalkBox.left += m_Velocity.x * elapsedSec;
	m_WalkBox.bottom += m_Velocity.y * elapsedSec;
	m_Map->HandleCollision(m_WalkBox, m_Velocity, m_Ground);
	if (m_Interactable)
	{
		PickUp();
	}

	if (CheckWarp(transition))
	{
		if (m_Interactable)
		{
			m_Interactable->Destroy();
			m_Interactable = nullptr;
		}
		return false;
	}

	CheckStairs();

	m_Map = m_Map->MapChange(m_WalkBox);
	return false;
}

bool Player::CheckWarp(Transition& transition)
{
	WarpPoint* warpPoint{ m_Map->CheckWarp(m_WalkBox) };
	if (warpPoint)
	{
		transition.SetWarpPoint(warpPoint);
		return true;
	}
	return false;
}

void Player::CheckStairs()
{
	Stairs* stairs{ m_Map->CheckStairs(m_WalkBox) };
	if (stairs)
	{
		m_Ground = stairs->IsGround();
		if (m_Interactable)
		{
			m_Interactable->SetGround(stairs->IsGround());
		}
	}
}

bool Player::UpdateHit(float elapsedSec, ProjectileManager& projectileManager)
{
	m_HitTime += elapsedSec;
	m_Draw = !m_Draw;
	if (m_HitTime >= m_HitTimeEnd)
	{
		m_HitTime = 0.0f;
		m_Hit = false;
		m_Draw = true;
		return false;
	}
	if (m_Damage && m_HitTime >= m_DamageTimeEnd)
	{
		m_ActionState = ActionState::waiting;
		m_Damage = false;
	}
	return m_Damage;
}

void Player::PickUp()
{
	int direction{ static_cast<int>(m_Direction) };
	Vector2f offsetVector;
	if (m_ActionState == ActionState::pickUp)
	{
		offsetVector = Vector2f{ m_InteractableOffsets[direction][m_AnimFrame] };
	}
	else
	{
		offsetVector = Vector2f{ m_InteractableOffsets[direction].back() };
	}
	Point2f position{ m_WalkBox.left + m_WalkBox.width / 2, m_WalkBox.bottom };
	position += offsetVector;
	m_Interactable->SetPosition(position);
}

void Player::Throw(ProjectileManager& projectileManager)
{
	m_Interactable->Throw(m_Direction, m_InteractableHeight);
	projectileManager.AddPlayerProjectile(m_Interactable);
	m_Interactable = nullptr;
}

void Player::DestroyInteractable(ProjectileManager& projectileManager)
{
	m_Interactable->Destroying();
	projectileManager.AddPlayerProjectile(m_Interactable);
	m_Interactable = nullptr;
}

void Player::HitInteractables(ProjectileManager& projectileManager, PickUpManager& pickUpManager)
{
	std::vector<Point2f> hitbox{};
	if (SwordHitBox(hitbox))
	{
		m_Map->InteractableHit(hitbox, projectileManager, pickUpManager);
	}
}

void Player::Draw() const
{
	if (!m_Draw)
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

	if (m_Interactable)
	{
		m_Interactable->DrawPlayer();
	}

#if defined(DEBUG) | defined(_DEBUG)
	utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
	utils::DrawRect(m_WalkBox);

	utils::SetColor(Color4f{ 1.0f, 0.0f,0.0f,1.0f });
	utils::DrawRect(GetHitBox());
	if (m_ActionState == ActionState::sword)
	{
		utils::SetColor(Color4f{ 0.0f, 1.0f,1.0f,1.0f });
		std::vector<Point2f> hitbox{};
		SwordHitBox(hitbox);
		utils::DrawPolygon(hitbox);
	}
#endif
}

Rectf Player::GetHitBox() const
{
	Rectf hitBox{ m_WalkBox.left + m_HitBox.left, m_WalkBox.bottom + m_HitBox.bottom, m_HitBox.width, m_HitBox.height };
	return hitBox;
}

Rectf Player::GetWalkBox() const
{
	return m_WalkBox;
}

void Player::SetMap(Map* map)
{
	m_Map = map;
}

Map* Player::GetMap() const
{
	return m_Map;
}

bool Player::Hit(Direction direction, const int damage, ProjectileManager& projectileManager)
{
	if (m_Interactable)
	{
		DestroyInteractable(projectileManager);
	}
	if (m_Hit)
	{
		return false;
	}
	m_Health -= damage;
	if (m_Health <= 0)
	{
		Dying();
		return false;
	}
	if (m_HitSound)
	{
		m_HitSound->Play(0);
	}
	m_ActionState = ActionState::damage;
	m_Hit = true;
	m_Damage = true;
	switch (direction)
	{
	case Direction::south:
		m_Velocity.x = 0.0f;
		m_Velocity.y = -m_DamageSpeed;
		m_Direction = Direction::north;
		break;
	case Direction::west:
		m_Velocity.x = -m_DamageSpeed;
		m_Velocity.y = 0.0f;
		m_Direction = Direction::east;
		break;
	case Direction::north:
		m_Velocity.x = 0.0f;
		m_Velocity.y = m_DamageSpeed;
		m_Direction = Direction::south;
		break;
	case Direction::east:
		m_Velocity.x = m_DamageSpeed;
		m_Velocity.y = 0.0f;
		m_Direction = Direction::west;
		break;
	}
	return true;
}

bool Player::SwordHitBox(std::vector<Point2f>& hitbox) const
{
	if (m_ActionState != ActionState::sword)
	{
		return false;
	}
	int direction{ static_cast<int>(m_Direction) };
	hitbox = m_SwordHitBoxes[direction][m_AnimFrame];
	Point2f offset = m_SpriteOffsets[static_cast<int>(ActionState::sword)];
	Vector2f playerOffset{ m_WalkBox.left - offset.x, m_WalkBox.bottom - offset.y };
	for (Point2f& point2F : hitbox)
	{
		point2F += playerOffset;
	}
	return true;
}

Direction Player::GetDirection() const
{
	return m_Direction;
}

int Player::GetSwordDamage()
{
	return m_SwordDamage;
}

void Player::AddRupees(const int amount)
{
	m_Rupees += amount;
	if (m_Rupees > m_MaxRupees)
	{
		m_Rupees = m_MaxRupees;
	}
}

void Player::AddHealth(const int amount)
{
	m_Health += amount;
	if (m_Health > m_MaxHealth)
	{
		m_Health = m_MaxHealth;
	}
}

bool Player::IsGround() const
{
	return m_Ground;
}

void Player::Warp(WarpPoint* warpPoint)
{
	if (warpPoint)
	{
		m_Map = warpPoint->GetMap();
		m_Ground = warpPoint->IsGround();
		Point2f location = warpPoint->GetLocation();
		m_WalkBox.left = location.x;
		m_WalkBox.bottom = location.y;
		m_Direction = warpPoint->GetDirection();
		m_ActionState = ActionState::waiting;
		ResetAnimation();
	}
}

void Player::InitDeathWarp(Map* map)
{
	m_DeathWarp = new WarpPoint{ Rectf{}, map, Point2f{m_WalkBox.left, m_WalkBox.bottom}, m_Ground, Direction::south };
}

int Player::GetHealth() const
{
	return m_Health;
}

int Player::GetMaxHealth() const
{
	return m_MaxHealth;
}

int Player::GetRupees() const
{
	return m_Rupees;
}

void Player::ProcessKeyBoardState(PickUpManager& pickUpManager)
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	if (pStates[SDL_SCANCODE_K])
	{
		if (m_Interactable)
		{
			if (Throw())
			{
				return;
			}
		}
		m_ActionState = ActionState::sword;
		if (m_SwordSound)
		{
			m_SwordSound->Play(0);
		}
		return;
	}
	if (pStates[SDL_SCANCODE_L])
	{
		if (m_Interactable)
		{
			if (Throw())
			{
				return;
			}
		}
		else
		{
			m_Interactable = m_Map->PickUp(m_WalkBox, m_Direction, pickUpManager);
			if (m_Interactable)
			{
				m_ActionState = ActionState::pickUp;
				if (m_PickUpSound)
				{
					m_PickUpSound->Play(0);
				}
				return;
			}
		}
	}
	if (pStates[SDL_SCANCODE_W] && !pStates[SDL_SCANCODE_S])
	{
		m_Velocity.y = m_WalkSpeed;
		m_Direction = Direction::north;
		if (!m_Interactable)
		{
			m_ActionState = ActionState::moving;
		}
		else
		{
			m_ActionState = ActionState::movingPickUp;
		}
	}
	else if (!pStates[SDL_SCANCODE_W] && pStates[SDL_SCANCODE_S])
	{
		m_Velocity.y = -m_WalkSpeed;
		m_Direction = Direction::south;
		if (!m_Interactable)
		{
			m_ActionState = ActionState::moving;
		}
		else
		{
			m_ActionState = ActionState::movingPickUp;
		}
	}
	if (pStates[SDL_SCANCODE_A] && !pStates[SDL_SCANCODE_D])
	{
		m_Velocity.x = -m_WalkSpeed;
		m_Direction = Direction::west;
		if (!m_Interactable)
		{
			m_ActionState = ActionState::moving;
		}
		else
		{
			m_ActionState = ActionState::movingPickUp;
		}
	}
	else if (!pStates[SDL_SCANCODE_A] && pStates[SDL_SCANCODE_D])
	{
		m_Velocity.x = m_WalkSpeed;
		m_Direction = Direction::east;
		if (!m_Interactable)
		{
			m_ActionState = ActionState::moving;
		}
		else
		{
			m_ActionState = ActionState::movingPickUp;
		}
	}

	if (m_Velocity.x == 0.0f && m_Velocity.y == 0.0f)
	{
		if (m_Interactable)
		{
			m_ActionState = ActionState::waitingPickUp;
		}
		else
		{
			m_ActionState = ActionState::waiting;
		}
	}
}

bool Player::Throw()
{
	if (m_ActionState == ActionState::movingPickUp || m_ActionState == ActionState::waitingPickUp)
	{
		m_ActionState = ActionState::throwing;
		if (m_ThrowSound)
		{
			m_ThrowSound->Play(0);
		}
		return true;
	}
	return false;
}

bool Player::UpdateFrame(float elapsedSec)
{
	m_AnimTime += elapsedSec;
	int actionState{ static_cast<int>(m_ActionState) };
	int direction{ static_cast<int>(m_Direction) };
	int nrOfFrames = m_NrOfFrames[actionState][direction];
	float frameTime{ m_AnimationTime[actionState][direction] / nrOfFrames };
	if (m_AnimTime >= frameTime)
	{
		m_AnimTime -= frameTime;
		++m_AnimFrame;
		m_AnimFrame = m_AnimFrame % nrOfFrames;
		if (m_AnimFrame == 0)
		{
			if (m_ActionState == ActionState::pickUp)
			{
				m_ActionState = ActionState::waitingPickUp;
			}
			if (m_ActionState == ActionState::sword || m_ActionState == ActionState::throwing)
			{
				m_ActionState = ActionState::waiting;
			}
			return false;
		}
	}
	return true;
}

void Player::ResetAnimation()
{
	m_AnimFrame = 0;
	m_AnimTime = 0.0f;
}

void Player::InitializeNrOfFrames()
{
	std::vector<int> waiting{ 1,1,1,1,1 };
	std::vector<int> moving{ 7,6,8,6,8 };
	std::vector<int> sword{ 6,5,5,5,6 };
	std::vector<int> boomerang{ 2,2,2,2,2 };
	std::vector<int> net{ 2,2,2,2,2 };
	std::vector<int> bow{ 3,3,3,3,3 };
	std::vector<int> pickUp{ 5,5,5,5,5 };
	std::vector<int> waitingPickUp{ 1,1,1,1,1 };
	std::vector<int> movingPickUp{ 6,3,6,3,6 };
	std::vector<int> throwing{ 1,1,1,1,1 };
	std::vector<int> damage{ 1,1,1,1,1 };
	std::vector<int> dying{ 1,1,1,1,1 };
	std::vector<int> death{ 2,2,2,2,2 };
	m_NrOfFrames = { waiting, moving, sword, boomerang, net, bow, pickUp, waitingPickUp, movingPickUp, throwing, damage, dying, death };
}

void Player::InitializeAnimationTime()
{
	std::vector<float> waiting{ 0.1f, 0.1f, 0.1f, 0.1f };
	std::vector<float> moving{ 0.75f, 0.75f, 0.75f, 0.75 };
	std::vector<float> sword{ 0.4f, 0.4f, 0.4f, 0.4f };
	std::vector<float> boomerang{ 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<float> net{ 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<float> bow{ 1.0f, 1.0f, 1.0f, 1.0f };
	std::vector<float> pickUp{ 0.5f, 0.5f, 0.5f, 0.5f };
	std::vector<float> waitingPickUp{ 0.1f, 0.1f, 0.1f, 0.1f };
	std::vector<float> movingPickUp{ 0.75f, 0.25f, 0.75f, 0.25f };
	std::vector<float> throwing{ 0.1f, 0.1f, 0.1f, 0.1f };
	std::vector<float> damage{ 0.1f, 0.1f, 0.1f, 0.1f };
	std::vector<float> dying{ 1.1f, 1.1f, 1.1f, 1.1f };
	std::vector<float> death{ 0.1f, 0.1f, 0.1f, 0.1f };
	m_AnimationTime = { waiting, moving, sword, boomerang, net, bow, pickUp, waitingPickUp, movingPickUp, throwing, damage, dying, death };
}

void Player::InitializeTextures(TextureManager& textureManager)
{
	std::string link{ "Resources/Link/" };
	std::string ext{ ".png" };
	std::string shield{ "" };
	if (m_Shield)
	{
		shield = "Shield";
	}
	Texture* waiting{ textureManager.CreateTexture(link + "Waiting" + shield + ext) };
	Texture* moving{ textureManager.CreateTexture(link + "Moving" + shield + ext) };
	Texture* sword{ textureManager.CreateTexture(link + "Sword" + ext) };
	Texture* boomerang{ textureManager.CreateTexture(link + "Boomerang" + ext) };
	Texture* net{ textureManager.CreateTexture(link + "Net" + ext) };
	Texture* bow{ textureManager.CreateTexture(link + "Bow" + ext) };
	Texture* pickUp{ textureManager.CreateTexture(link + "PickUp" + ext) };
	Texture* waitingPickUp{ textureManager.CreateTexture(link + "WaitingPickUp" + ext) };
	Texture* movingPickUp{ textureManager.CreateTexture(link + "MovingPickUp" + ext) };
	Texture* throwing{ textureManager.CreateTexture(link + "Throwing" + ext) };
	Texture* damage{ textureManager.CreateTexture(link + "Damage" + ext) };
	Texture* dying{ textureManager.CreateTexture(link + "Dying" + ext) };
	Texture* death{ textureManager.CreateTexture(link + "Death" + ext) };
	m_SpriteTextures.push_back(waiting);
	m_SpriteTextures.push_back(moving);
	m_SpriteTextures.push_back(sword);
	m_SpriteTextures.push_back(boomerang);
	m_SpriteTextures.push_back(net);
	m_SpriteTextures.push_back(bow);
	m_SpriteTextures.push_back(pickUp);
	m_SpriteTextures.push_back(waitingPickUp);
	m_SpriteTextures.push_back(movingPickUp);
	m_SpriteTextures.push_back(throwing);
	m_SpriteTextures.push_back(damage);
	m_SpriteTextures.push_back(dying);
	m_SpriteTextures.push_back(death);
}

void Player::InitializeSpriteOffset()
{
	Point2f waiting{ 4, 0 };
	Point2f moving{ 4, 0 };
	Point2f sword{ 18, 13 };
	Point2f boomerang{ 8, 1 };
	Point2f net{ 18, 13 };
	Point2f bow{ 7, 2 };
	Point2f pickUp{ 12, 0 };
	Point2f waitingPickUp{ 4, 0 };
	Point2f movingPickUp{ 4, 1 };
	Point2f throwing{ 4, 0 };
	Point2f damage{ 2, 0 };
	Point2f dying{ 4, 0 };
	Point2f death{ 3, 1 };
	m_SpriteOffsets = { waiting, moving, sword, boomerang, net, bow, pickUp, waitingPickUp, movingPickUp, throwing, damage, dying, death };

	std::vector<Vector2f> south{ Vector2f{0.0f, 0.0f}, Vector2f{0.0f, 5.0f}, Vector2f{0.0f, 10.0f} , Vector2f{0.0f, 15.0f} , Vector2f{0.0f, 20.0f}, Vector2f{0.0f, 17.0f} };
	std::vector<Vector2f> west{ Vector2f{-12.0f, 2.0f} , Vector2f{-12.0f, 5.0f} , Vector2f{-12.0f, 10.0f} , Vector2f{-6.0f, 15.0f} , Vector2f{0.0f, 20.0f}, Vector2f{0.0f, 17.0f} };
	std::vector<Vector2f> north{ Vector2f{0.0f, 12.0f} , Vector2f{0.0f, 10.0f} , Vector2f{0.0f, 12.0f} , Vector2f{0.0f, 15.0f} , Vector2f{0.0f, 20.0f}, Vector2f{0.0f, 17.0f} };
	std::vector<Vector2f> east{ Vector2f{12.0f, 2.0f} , Vector2f{12.0f, 5.0f} , Vector2f{12.0f, 10.0f} , Vector2f{6.0f, 15.0f} , Vector2f{0.0f, 20.0f}, Vector2f{0.0f, 17.0f} };
	m_InteractableOffsets = { south, west, north, east };
}

void Player::InitializeSwordHitBoxes()
{
	int actionState{ static_cast<int>(ActionState::sword) };
	for (int direction = 0; direction < m_NrOfDirections; ++direction)
	{
		std::vector<std::vector<Point2f>> directionHitBoxes{};
		int nrOfFrames{ m_NrOfFrames[actionState][direction] };
		for (int frame = 0; frame < nrOfFrames; ++frame)
		{
			std::vector<std::vector<Point2f>> vertices{};
			SVGParser::GetVerticesFromSvgFile("Resources/Link/Sword/" + std::to_string(direction) + "/" + std::to_string(frame) + ".svg", vertices);
			directionHitBoxes.push_back(vertices[0]);
		}
		m_SwordHitBoxes.push_back(directionHitBoxes);
	}
}

void Player::InitializeSounds(SoundManager& soundManager)
{
	std::string link{ "Resources/Link/" };
	std::string ext{ ".wav" };
	m_DeathSound = soundManager.CreateSoundEffect(link + "Death" + ext);
	m_DyingSound = soundManager.CreateSoundEffect(link + "Dying" + ext);
	m_HitSound = soundManager.CreateSoundEffect(link + "Hit" + ext);
	m_LowHpSound = soundManager.CreateSoundEffect(link + "LowHp" + ext);
	m_PickUpSound = soundManager.CreateSoundEffect(link + "PickUp" + ext);
	m_SwordSound = soundManager.CreateSoundEffect(link + "Sword" + ext);
	m_ThrowSound = soundManager.CreateSoundEffect(link + "Throwing" + ext);
}

void Player::Dying()
{
	m_ActionState = ActionState::dying;
	m_SoundManager.StopSoundEffects();
	m_SoundManager.StopSoundStream();
	if (m_DyingSound)
	{
		m_DyingSound->Play(0);
	}
	m_DyingFrames = m_DyingFramesStart + static_cast<int>(m_Direction);
	ResetAnimation();
}

void Player::UpdateDying(float elapsedSec)
{
	m_AnimTime += elapsedSec;
	int actionState{ static_cast<int>(ActionState::dying) };
	int direction{ static_cast<int>(m_Direction) };
	float frameTime{ m_AnimationTime[actionState][direction] / m_DyingFrames };
	if (m_AnimTime >= frameTime)
	{
		m_AnimTime -= frameTime;
		++m_DyingFrame;
		m_DyingFrame %= m_DyingFrames;
		if (m_DyingFrame == 0)
		{
			m_ActionState = ActionState::death;
			ResetAnimation();
			return;
		}
		DyingRotation();
	}
}

void Player::DyingRotation()
{
	if (m_Direction == Direction::south)
	{
		m_Direction = Direction::east;
	}
	else
	{
		m_Direction = static_cast<Direction>(static_cast<int>(m_Direction) - 1);
	}
}

void Player::UpdateDeath(float elapsedSec, Transition& transition)
{
	m_AnimTime += elapsedSec;
	int actionState{ static_cast<int>(m_ActionState) };
	int direction{ static_cast<int>(m_Direction) };
	int nrOfFrames = m_NrOfFrames[actionState][direction];
	float frameTime{ m_AnimationTime[actionState][direction] / nrOfFrames };
	if (m_AnimTime >= frameTime)
	{
		++m_AnimFrame;
		m_AnimTime -= frameTime;
		if (m_AnimFrame == nrOfFrames - 1)
		{
			if (m_DeathSound)
			{
				m_DeathSound->Play(0);
			}
		}
		if (m_AnimFrame == nrOfFrames)
		{
			Death(transition);
			m_AnimFrame -= 1;
		}
	}
}

void Player::Death(Transition& transition)
{
	transition.SetWarpPoint(m_DeathWarp);
	m_Health = m_MaxHealth;
}
