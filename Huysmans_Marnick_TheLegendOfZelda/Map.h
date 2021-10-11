#pragma once
#include <vector>
#include "Texture.h"
#include "Interactable.h"
#include "WarpPoint.h"
#include "Stairs.h"

class ProjectileManager;
class Floor;
struct Vector2f;
class Level;

class Map final
{
public:
	static void UpdateFrame(float elapsedSec);

	Map(const int id, std::vector<std::vector<Point2f>>& vertices, std::vector<std::vector<Point2f>>& verticesUp, const Texture* backGround, const Texture* top, const Texture* up, const std::vector<Texture*> animations, const Rectf& boundaries);
	~Map();

	Map(const Map& other) = delete;
	Map(Map&& other) noexcept = delete;
	Map& operator=(const Map& other) = delete;
	Map& operator=(Map&& other) noexcept = delete;

	void DrawLevelBackGround(const Rectf& cameraTransform) const;
	void DrawLevelTop(const Rectf& cameraTransform) const;
	void DrawLevelUp(const Rectf& cameraTransform) const;
	void DrawBackGround() const;
	void DrawTop() const;
	void DrawUp() const;
	void DrawInteractables() const;
	bool HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, const bool ground) const;
	bool CheckOverlap(const Rectf& actorShape, const bool ground) const;
	void InteractableHit(const std::vector<Point2f>& hitBox, ProjectileManager& projectileManager, PickUpManager& pickUpManager);
	Interactable* PickUp(const Rectf& actorShape, Direction direction, PickUpManager& pickUpManager);
	Rectf GetBoundaries() const;
	void SetNorth(Map* const map);
	void SetEast(Map* const map);
	void SetSouth(Map* const map);
	void SetWest(Map* const map);
	Map* GetNorth() const;
	Map* GetEast() const;
	Map* GetSouth() const;
	Map* GetWest() const;
	Map* MapChange(const Rectf& moveRect);
	void AddInteractable(Interactable* interactable);
	void AddWarpPoint(WarpPoint* warpPoint);
	WarpPoint* CheckWarp(const Rectf& actorShape);
	void AddStairs(Stairs* stairs);
	Stairs* CheckStairs(const Rectf& actorShape);
	void SetFloor(Floor* floor);
	void SetCurrentFloor();
	Floor* GetFloor() const;
	int GetId() const;
	void Reset();
private:
	static const int m_NrOfFrames{ 3 };
	static const int m_NrFramesPerSec{ 10 };
	static float m_AnimTime;
	static int m_AnimFrame;

	const int m_Id;
	const std::vector<std::vector<Point2f>> m_Vertices;
	const std::vector<std::vector<Point2f>> m_VerticesUp;
	const Texture* m_BackGround;
	const Texture* m_Top;
	const Texture* m_Up;
	const std::vector<Texture*> m_Animations;
	std::vector<Interactable*> m_Interactables;
	Floor* m_Floor;
	std::vector <WarpPoint*> m_WarpPoints;
	std::vector<Stairs*> m_Stairs;
	Rectf m_Boundaries;
	Map* m_North;
	Map* m_East;
	Map* m_South;
	Map* m_West;
	const float m_HorOffset{ 1.0f };
	const float m_VerOffset{ 0.5f };

	bool HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const;
	bool DiagonalMovement(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const;
	bool HorMovement(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const;
	bool VerMovement(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const;
	bool HitDiagonal(Rectf& actorShape, Vector2f& actorVelocity, Point2f& ray1, Point2f& ray1End, Point2f& ray2, Point2f& ray2End, Point2f& ray3, Point2f& ray3End, const std::vector<std::vector<Point2f>>& vertices) const;
	bool HitHor(Rectf& actorShape, Vector2f& actorVelocity, Point2f& ray1, Point2f& ray1End, Point2f& ray2, Point2f& ray2End, const std::vector<std::vector<Point2f>>& vertices) const;
	bool HitVer(Rectf& actorShape, Vector2f& actorVelocity, Point2f& ray1, Point2f& ray1End, Point2f& ray2, Point2f& ray2End, const std::vector<std::vector<Point2f>>& vertices) const;
	void CollideActorDiagonal(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint, const Vector2f normal) const;
	void CollideActorHor(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint1, const Point2f& intersectionPoint2) const;
	void CollideActorVer(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint1, const Point2f& intersectionPoint2) const;
	void CollideActorX(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint) const;
	void CollideActorY(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint) const;
	bool CollideInteractable(Rectf& actorShape, Vector2f& actorVelocity) const;
	void DrawBorderingBackGround(const Rectf& cameraTransform) const;
	void DrawBorderingTop(const Rectf& cameraTransform) const;
	void DrawBorderingUp(const Rectf& cameraTransform) const;
	std::vector<Map*> GetBordering(const Rectf& cameraTransform) const;
};
