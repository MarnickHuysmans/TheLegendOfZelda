#pragma once
#include <vector>
#include "Texture.h"
#include "Vector2f.h"
#include "Interactable.h"
#include "Direction.h"
#include "SoundEffect.h"

class TextureManager;
class SoundManager;
class Transition;
class WarpPoint;
class ProjectileManager;
class Map;
class Level;

class Player final
{
public:
	enum class ActionState
	{
		waiting = 0,
		moving = 1,
		sword = 2,
		boomerang = 3,
		net = 4,
		bow = 5,
		pickUp = 6,
		waitingPickUp = 7,
		movingPickUp = 8,
		throwing = 9,
		damage = 10,
		dying = 11,
		death = 12
	};
	Player(SoundManager& soundManager, TextureManager& textureManager);
	~Player();

	Player(const Player& other) = delete;
	Player(Player&& other) noexcept = delete;
	Player& operator=(const Player& other) = delete;
	Player& operator=(Player&& other) noexcept = delete;

	bool Update(float elapsedSec, ProjectileManager& projectileManager, Transition& transition, PickUpManager& pickUpManager);
	void DrawGround() const;
	void DrawUp() const;
	void Draw() const;
	Rectf GetHitBox() const;
	Rectf GetWalkBox() const;
	void SetMap(Map* map);
	Map* GetMap() const;
	bool Hit(Direction direction, const int damage, ProjectileManager& projectileManager);
	bool SwordHitBox(std::vector<Point2f>& hitbox) const;
	Direction GetDirection() const;
	int GetSwordDamage();
	void AddRupees(const int amount);
	void AddHealth(const int amount);
	bool IsGround() const;
	void Warp(WarpPoint* warpPoint);
	void InitDeathWarp(Map* map);
	int GetHealth() const;
	int GetMaxHealth() const;
	int GetRupees() const;

private:
	int m_MaxHealth;
	int m_Health;
	Rectf m_WalkBox;
	Rectf m_HitBox;
	bool m_Ground;
	Map* m_Map;
	bool m_Shield;
	const int m_NrOfDirections{ 4 };
	const float m_WalkSpeed{ 75.0f };
	const float m_DamageSpeed{ 125.0f };
	Vector2f m_Velocity;
	Direction m_Direction;
	ActionState m_ActionState;
	std::vector<std::vector<int>> m_NrOfFrames;
	std::vector<Texture*> m_SpriteTextures;
	std::vector<std::vector<std::vector<Point2f>>> m_SwordHitBoxes;
	const int m_SwordDamage{ 1 };
	std::vector<Point2f> m_SpriteOffsets;
	std::vector<std::vector<float>> m_AnimationTime;
	float m_AnimTime;
	int m_AnimFrame;
	Interactable* m_Interactable;
	const float m_InteractableHeight{ 17.0f };
	std::vector<std::vector<Vector2f>> m_InteractableOffsets;
	bool m_Hit;
	bool m_Damage;
	bool m_Draw;
	const float m_DamageTimeEnd{ 0.25f };
	const float m_HitTimeEnd{ 1.0f };
	float m_HitTime;
	int m_Rupees;
	const int m_MaxRupees{ 999 };
	WarpPoint* m_DeathWarp;
	const float m_DyingTime{ 1.1f };
	const int m_DyingFramesStart{ 5 };
	int m_DyingFrames;
	int m_DyingFrame;
	SoundManager& m_SoundManager;

	SoundEffect* m_DeathSound;
	SoundEffect* m_DyingSound;
	SoundEffect* m_HitSound;
	SoundEffect* m_LowHpSound;
	SoundEffect* m_PickUpSound;
	SoundEffect* m_SwordSound;
	SoundEffect* m_ThrowSound;

	void ProcessKeyBoardState(PickUpManager& pickUpManager);
	bool Throw();
	bool UpdateFrame(float elapsedSec);
	void ResetAnimation();
	bool Moving(float elapsedSec, ProjectileManager& projectileManager, Transition& transition);
	bool CheckWarp(Transition& transition);
	void CheckStairs();
	bool UpdateHit(float elapsedSec, ProjectileManager& projectileManager);
	void PickUp();
	void Throw(ProjectileManager& projectileManager);
	void DestroyInteractable(ProjectileManager& projectileManager);
	void HitInteractables(ProjectileManager& projectileManager, PickUpManager& pickUpManager);
	void InitializeNrOfFrames();
	void InitializeAnimationTime();
	void InitializeTextures(TextureManager& textureManager);
	void InitializeSpriteOffset();
	void InitializeSwordHitBoxes();
	void InitializeSounds(SoundManager& soundManager);
	void Dying();
	void UpdateDying(float elapsedSec);
	void DyingRotation();
	void UpdateDeath(float elapsedSec, Transition& transition);
	void Death(Transition& transition);
};
