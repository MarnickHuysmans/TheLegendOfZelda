#pragma once
#include "Projectile.h"
#include <vector>
#include "Vector2f.h"
class PickUpManager;
enum class Direction;

class Interactable : public Projectile
{
public:	
	Interactable(const Rectf& boundaries, const int destroyFrames, const Point2f&& destroyOffset, const int framesPerSec, const float fallSpeed, const float throwSpeed, const float flyingOffset, const int damage, const bool ground);
	virtual ~Interactable() {}
	virtual void DrawMap() const;
	virtual void DrawPlayer() const;
	bool Update(float elapsedSec, const Map* const map) override;
	virtual bool Interact(const Rectf& actorShape, Direction direction, PickUpManager& pickUpManager);
	virtual bool SwordHit(const std::vector<Point2f>& hitBox, PickUpManager& pickUpManager);
	virtual void Throw(Direction direction, float height);
	virtual void Destroying();
	virtual void Destroy();
	virtual void SetPosition(Point2f& positionBottomCenter) = 0;
	virtual void Reset();

	virtual void HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const;

	bool Hit(const Rectf& hitBox) override;

protected:
	Rectf m_StartBoundaries;
	Rectf m_Boundaries;
	const float m_HorOffset{ 1.0f };
	const float m_VerOffset{ 0.5f };

	const int m_DestroyFrames;
	const Point2f m_DestroyOffset;
	const int m_NrFramesPerSec;
	const float m_FallSpeed;
	const float m_ThrowSpeed;
	const float m_FlyingOffset;
	
	bool m_Destroyed;
	bool m_PickUp;
	bool m_Throwing;
	bool m_Destroying;
	float m_AnimTime;
	int m_AnimFrame;
	float m_CurrentHeight;

	virtual bool Destroying(float elapsedSec);

private:
	void DiagonalMovement(Rectf& actorShape, Vector2f& actorVelocity, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const;
	void HorMovement(Rectf& actorShape, Vector2f& actorVelocity, float& collideXHor) const;
	void VerMovement(Rectf& actorShape, Vector2f& actorVelocity, float& collideYVer) const;
	void HitUpRight(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const;
	void HitDownRight(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const;
	void HitUpLeft(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const;
	void HitDownLeft(Rectf& actorShape, float& collideXDiagonal, float& collideYDiagonal, float& collideXHor, float& collideYVer) const;
	void HitRight(Rectf& actorShape, float& collideXHor) const;
	void HitLeft(Rectf& actorShape, float& collideXHor) const;
	void HitUp(Rectf& actorShape, float& collideYVer) const;
	void HitDown(Rectf& actorShape, float& collideYVer) const;
	void CollideRight(Rectf& actorShape, float& collide) const;
	void CollideLeft(float& collide) const;
	void CollideUp(Rectf& actorShape, float& collide) const;
	void CollideDown(float& collide) const;

	virtual void DropPickUp(PickUpManager& pickUpManager) = 0;
};

