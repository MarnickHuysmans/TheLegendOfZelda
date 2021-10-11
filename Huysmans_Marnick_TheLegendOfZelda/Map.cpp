#include "pch.h"
#include "Map.h"
#include "utils.h"
#include "Floor.h"
#include "ProjectileManager.h"

float Map::m_AnimTime{ 0.0f };
int Map::m_AnimFrame{ 0 };

void Map::UpdateFrame(float elapsedSec)
{
	m_AnimTime += elapsedSec;
	float frameTime{ 1.0f / m_NrFramesPerSec };
	if (m_AnimTime >= frameTime)
	{
		m_AnimTime -= frameTime;
		++m_AnimFrame;
		m_AnimFrame %= m_NrOfFrames;
	}
}

Map::Map(const int id, std::vector<std::vector<Point2f>>& vertices, std::vector<std::vector<Point2f>>& verticesUp, const Texture* backGround, const Texture* top, const Texture* up, const std::vector<Texture*> animations, const Rectf& boundaries)
	: m_Id(id)
	, m_Vertices(std::move(vertices))
	, m_VerticesUp(std::move(verticesUp))
	, m_BackGround(backGround)
	, m_Top(top)
	, m_Up(up)
	, m_Animations(animations)
	, m_Floor(nullptr)
	, m_Boundaries(boundaries)
	, m_North(nullptr)
	, m_East(nullptr)
	, m_South(nullptr)
	, m_West(nullptr)
{
}

Map::~Map()
{
	for (Interactable* interactable : m_Interactables)
	{
		delete interactable;
	}
	for (WarpPoint* warpPoint : m_WarpPoints)
	{
		delete warpPoint;
	}
	for (Stairs* stairs : m_Stairs)
	{
		delete stairs;
	}
}

void Map::DrawLevelBackGround(const Rectf& cameraTransform) const
{
	DrawBackGround();
	DrawBorderingBackGround(cameraTransform);
}

void Map::DrawLevelTop(const Rectf& cameraTransform) const
{
	DrawTop();
	DrawBorderingTop(cameraTransform);
}

void Map::DrawLevelUp(const Rectf& cameraTransform) const
{
	DrawUp();
	DrawBorderingUp(cameraTransform);
}

void Map::DrawBackGround() const
{
	m_BackGround->Draw(m_Boundaries);
	if (static_cast<unsigned>(m_AnimFrame) < m_Animations.size())
	{
		m_Animations[m_AnimFrame]->Draw(m_Boundaries);
	}
	DrawInteractables();
#if defined(DEBUG) | defined(_DEBUG)
	utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
	for (const std::vector<Point2f>& vector : m_Vertices)
	{
		utils::DrawPolygon(vector);
	}
	for (const std::vector<Point2f>& vector : m_VerticesUp)
	{
		utils::DrawPolygon(vector);
	}
	utils::SetColor(Color4f{ 0.0f, 0.0f, 1.0f, 1.0f });
	for (WarpPoint* const warpPoint : m_WarpPoints)
	{
		utils::DrawRect(warpPoint->GetBoundaries());
	}
	utils::SetColor(Color4f{ 0.0f, 1.0f, 0.0f, 1.0f });
	for (Stairs* const stairs : m_Stairs)
	{
		utils::DrawRect(stairs->GetBoundaries());
	}
#endif
}

void Map::DrawTop() const
{
	if (m_Top)
	{
		m_Top->Draw(m_Boundaries);
	}
}

void Map::DrawUp() const
{
	if (m_Up)
	{
		m_Up->Draw(m_Boundaries);
	}
}

void Map::DrawInteractables() const
{
	for (const Interactable* const interactable : m_Interactables)
	{
		interactable->DrawMap();
	}
}

bool Map::HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, const bool ground) const
{
	if (ground)
	{
		return HandleCollision(actorShape, actorVelocity, m_Vertices);
	}
	return HandleCollision(actorShape, actorVelocity, m_VerticesUp);
}

bool Map::CheckOverlap(const Rectf& actorShape, const bool ground) const
{
	if (ground)
	{
		for (const std::vector<Point2f>& polygon : m_Vertices)
		{
			if (utils::IsOverlapping(polygon, actorShape))
			{
				return true;
			}
		}
	}
	else
	{
		for (const std::vector<Point2f>& polygon : m_VerticesUp)
		{
			if (utils::IsOverlapping(polygon, actorShape))
			{
				return true;
			}
		}
	}
	return false;
}

void Map::InteractableHit(const std::vector<Point2f>& hitBox, ProjectileManager& projectileManager, PickUpManager& pickUpManager)
{
	for (Interactable* const interactable : m_Interactables)
	{
		if (interactable->SwordHit(hitBox, pickUpManager))
		{
			projectileManager.AddPlayerProjectile(interactable);
		}
	}
}

Interactable* Map::PickUp(const Rectf& actorShape, Direction direction, PickUpManager& pickUpManager)
{
	for (Interactable* const interactable : m_Interactables)
	{
		if (interactable->Interact(actorShape, direction, pickUpManager))
		{
			return interactable;
		}
	}
	return nullptr;
}

Rectf Map::GetBoundaries() const
{
	return m_Boundaries;
}

void Map::SetNorth(Map* const map)
{
	m_North = map;
}

void Map::SetEast(Map* const map)
{
	m_East = map;
}

void Map::SetSouth(Map* const map)
{
	m_South = map;
}

void Map::SetWest(Map* const map)
{
	m_West = map;
}

Map* Map::GetNorth() const
{
	return m_North;
}

Map* Map::GetEast() const
{
	return m_East;
}

Map* Map::GetSouth() const
{
	return m_South;
}

Map* Map::GetWest() const
{
	return m_West;
}

bool Map::HandleCollision(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const
{
	bool diagonal{ actorVelocity.x != 0.0f && actorVelocity.y != 0.0f };
	bool collision{ false };
	if (diagonal)
	{
		collision = DiagonalMovement(actorShape, actorVelocity, vertices);
	}
	else
	{
		if (actorVelocity.x != 0.0f)
		{
			collision = HorMovement(actorShape, actorVelocity, vertices);
		}
		else
		{
			collision = VerMovement(actorShape, actorVelocity, vertices);
		}
	}

	bool collisionInteractable = CollideInteractable(actorShape, actorVelocity);
	return collision || collisionInteractable;
}

bool Map::DiagonalMovement(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const
{
	Point2f center{ actorShape.left + actorShape.width / 2, actorShape.bottom + actorShape.height / 2 };
	float x{};
	float xVer{};
	if (actorVelocity.x > 0)
	{
		x = actorShape.left + actorShape.width;
		xVer = actorShape.left;
	}
	else
	{
		x = actorShape.left;
		xVer = actorShape.left + actorShape.width;
	}
	float y{};
	float yHor{};
	if (actorVelocity.y > 0)
	{
		y = actorShape.bottom + actorShape.height;
		yHor = actorShape.bottom;
	}
	else
	{
		y = actorShape.bottom;
		yHor = actorShape.bottom + actorShape.height;
	}
	Point2f ray1 = Point2f{ center.x, center.y };
	Point2f ray1End = Point2f{ x, y };
	Point2f ray2 = Point2f{ center.x, yHor };
	Point2f ray2End = Point2f{ x, yHor };
	Point2f ray3 = Point2f{ xVer, center.y };
	Point2f ray3End = Point2f{ xVer, y };
	return HitDiagonal(actorShape, actorVelocity, ray1, ray1End, ray2, ray2End, ray3, ray3End, vertices);
}

bool Map::HorMovement(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const
{
	Point2f center{ actorShape.left + actorShape.width / 2, actorShape.bottom + actorShape.height / 2 };
	float bottom{ actorShape.bottom + m_VerOffset };
	float top{ actorShape.bottom + actorShape.height - m_VerOffset };
	float x{};
	if (actorVelocity.x > 0)
	{
		x = actorShape.left + actorShape.width;
	}
	else
	{
		x = actorShape.left;
	}
	Point2f ray1 = Point2f{ center.x, bottom };
	Point2f ray2 = Point2f{ center.x, top };
	Point2f ray1End = Point2f{ x, bottom };
	Point2f ray2End = Point2f{ x, top };
	return HitHor(actorShape, actorVelocity, ray1, ray1End, ray2, ray2End, vertices);
}

bool Map::VerMovement(Rectf& actorShape, Vector2f& actorVelocity, const std::vector<std::vector<Point2f>>& vertices) const
{
	Point2f center{ actorShape.left + actorShape.width / 2, actorShape.bottom + actorShape.height / 2 };
	float left{ actorShape.left + m_HorOffset };
	float right{ actorShape.left + actorShape.width - m_HorOffset };
	float y{};
	if (actorVelocity.y > 0)
	{
		y = actorShape.bottom + actorShape.height;
	}
	else
	{
		y = actorShape.bottom;
	}
	Point2f ray1 = Point2f{ left, center.y };
	Point2f ray2 = Point2f{ right, center.y };
	Point2f ray1End = Point2f{ left, y };
	Point2f ray2End = Point2f{ right, y };
	return HitVer(actorShape, actorVelocity, ray1, ray1End, ray2, ray2End, vertices);
}

bool Map::HitDiagonal(Rectf& actorShape, Vector2f& actorVelocity, Point2f& ray1, Point2f& ray1End, Point2f& ray2, Point2f& ray2End, Point2f& ray3,
	Point2f& ray3End, const std::vector<std::vector<Point2f>>& vertices) const
{
	bool collision{ false };
	utils::HitInfo hitInfo{};
	for (const std::vector<Point2f>& polygon : vertices)
	{
		if (utils::Raycast(polygon, ray1, ray1End, hitInfo))
		{
			CollideActorDiagonal(actorShape, actorVelocity, hitInfo.intersectPoint, hitInfo.normal);
			collision = true;
		}
		if (utils::Raycast(polygon, ray2, ray2End, hitInfo))
		{
			CollideActorX(actorShape, actorVelocity, hitInfo.intersectPoint);
			collision = true;
		}
		if (utils::Raycast(polygon, ray3, ray3End, hitInfo))
		{
			CollideActorY(actorShape, actorVelocity, hitInfo.intersectPoint);
			collision = true;
		}
	}
	return collision;
}

bool Map::HitHor(Rectf& actorShape, Vector2f& actorVelocity, Point2f& ray1, Point2f& ray1End, Point2f& ray2, Point2f& ray2End, const std::vector<std::vector<Point2f>>& vertices) const
{
	utils::HitInfo hitInfo1{};
	utils::HitInfo hitInfo2{};
	for (const std::vector<Point2f>& polygon : vertices)
	{
		bool hit1 = utils::Raycast(polygon, ray1, ray1End, hitInfo1);
		bool hit2 = utils::Raycast(polygon, ray2, ray2End, hitInfo2);
		if (hit1 && hit2)
		{
			CollideActorHor(actorShape, actorVelocity, hitInfo1.intersectPoint, hitInfo2.intersectPoint);
			return true;
		}
		if (hit1)
		{
			CollideActorX(actorShape, actorVelocity, hitInfo1.intersectPoint);
			return true;
		}
		if (hit2)
		{
			CollideActorX(actorShape, actorVelocity, hitInfo2.intersectPoint);
			return true;
		}
	}
	return false;
}

bool Map::HitVer(Rectf& actorShape, Vector2f& actorVelocity, Point2f& ray1, Point2f& ray1End, Point2f& ray2, Point2f& ray2End, const std::vector<std::vector<Point2f>>& vertices) const
{
	utils::HitInfo hitInfo1{};
	utils::HitInfo hitInfo2{};
	for (const std::vector<Point2f>& polygon : vertices)
	{
		bool hit1 = utils::Raycast(polygon, ray1, ray1End, hitInfo1);
		bool hit2 = utils::Raycast(polygon, ray2, ray2End, hitInfo2);
		if (hit1 && hit2)
		{
			CollideActorVer(actorShape, actorVelocity, hitInfo1.intersectPoint, hitInfo2.intersectPoint);
			return true;
		}
		if (hit1)
		{
			CollideActorY(actorShape, actorVelocity, hitInfo1.intersectPoint);
			return true;
		}
		if (hit2)
		{
			CollideActorY(actorShape, actorVelocity, hitInfo2.intersectPoint);
			return true;
		}
	}
	return false;
}

void Map::CollideActorDiagonal(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint, const Vector2f normal) const
{
	bool diagonal{ normal.x * normal.y != 0 };
	if (!diagonal)
	{
		if (normal.x != 0.0f)
		{
			CollideActorX(actorShape, actorVelocity, intersectionPoint);
		}
		else
		{
			CollideActorY(actorShape, actorVelocity, intersectionPoint);
		}
	}
	else
	{
		CollideActorX(actorShape, actorVelocity, intersectionPoint);
		CollideActorY(actorShape, actorVelocity, intersectionPoint);
	}
}

void Map::CollideActorHor(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint1, const Point2f& intersectionPoint2) const
{
	if (actorVelocity.x > 0)
	{
		if (intersectionPoint1.x <= intersectionPoint2.x)
		{
			actorShape.left = intersectionPoint1.x - actorShape.width;
		}
		else
		{
			actorShape.left = intersectionPoint2.x - actorShape.width;
		}
	}
	else if (actorVelocity.x < 0)
	{
		if (intersectionPoint1.x >= intersectionPoint2.x)
		{
			actorShape.left = intersectionPoint1.x;
		}
		else
		{
			actorShape.left = intersectionPoint2.x;
		}
	}
}

void Map::CollideActorVer(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint1, const Point2f& intersectionPoint2) const
{
	if (actorVelocity.y > 0)
	{
		if (intersectionPoint1.y <= intersectionPoint2.y)
		{
			actorShape.bottom = intersectionPoint1.y - actorShape.height;
		}
		else
		{
			actorShape.bottom = intersectionPoint2.y - actorShape.height;
		}
	}
	else if (actorVelocity.y < 0)
	{
		if (intersectionPoint1.y >= intersectionPoint2.y)
		{
			actorShape.bottom = intersectionPoint1.y;
		}
		else
		{
			actorShape.bottom = intersectionPoint2.y;
		}
	}
}


void Map::CollideActorX(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint) const
{
	if (actorVelocity.x > 0)
	{
		actorShape.left = intersectionPoint.x - actorShape.width;
	}
	else if (actorVelocity.x < 0)
	{
		actorShape.left = intersectionPoint.x;
	}
}

void Map::CollideActorY(Rectf& actorShape, Vector2f& actorVelocity, const Point2f& intersectionPoint) const
{
	if (actorVelocity.y > 0)
	{
		actorShape.bottom = intersectionPoint.y - actorShape.height;
	}
	else if (actorVelocity.y < 0)
	{
		actorShape.bottom = intersectionPoint.y;
	}
}

bool Map::CollideInteractable(Rectf& actorShape, Vector2f& actorVelocity) const
{
	float collideXDiagonal{ -1.0f };
	float collideYDiagonal{ -1.0f };
	float collideXHor{ -1.0f };
	float collideYVer{ -1.0f };
	for (const Interactable* const interactable : m_Interactables)
	{
		interactable->HandleCollision(actorShape, actorVelocity, collideXDiagonal, collideYDiagonal, collideXHor, collideYVer);
	}
	if (collideXHor > -1.0f)
	{
		actorShape.left = collideXHor;
		if (collideYVer > -1.0f)
		{
			actorShape.bottom = collideYVer;
		}
		return true;
	}
	if (collideYVer > -1.0f)
	{
		actorShape.bottom = collideYVer;
		return true;
	}
	if (collideXDiagonal > -1.0f)
	{
		if (actorVelocity.x > 0)
		{
			if (actorVelocity.y > 0)
			{
				if (collideYDiagonal - actorShape.bottom > collideXDiagonal - actorShape.left)
				{
					actorShape.bottom = collideYDiagonal;
				}
				else
				{
					actorShape.left = collideXDiagonal;
				}
			}
			else
			{
				if (actorShape.left - collideXDiagonal > collideYDiagonal - actorShape.bottom)
				{
					actorShape.bottom = collideYDiagonal;
				}
				else
				{
					actorShape.left = collideXDiagonal;
				}
			}
		}
		else
		{
			if (actorVelocity.y > 0)
			{
				if (collideYDiagonal - actorShape.bottom > actorShape.left - collideXDiagonal)
				{
					actorShape.bottom = collideYDiagonal;
				}
				else
				{
					actorShape.left = collideXDiagonal;
				}
			}
			else
			{
				if (collideXDiagonal - actorShape.left > collideYDiagonal - actorShape.bottom)
				{
					actorShape.bottom = collideYDiagonal;
				}
				else
				{
					actorShape.left = collideXDiagonal;
				}
			}
		}
		return true;
	}
	return false;
}

void Map::DrawBorderingBackGround(const Rectf& cameraTransform) const
{
	std::vector<Map*> borderingMaps{ GetBordering(cameraTransform) };
	for (const Map* const borderingMap : borderingMaps)
	{
		borderingMap->DrawBackGround();
	}
}

void Map::DrawBorderingTop(const Rectf& cameraTransform) const
{
	std::vector<Map*> borderingMaps{ GetBordering(cameraTransform) };
	for (const Map* const borderingMap : borderingMaps)
	{
		borderingMap->DrawTop();
	}
}

void Map::DrawBorderingUp(const Rectf& cameraTransform) const
{
	std::vector<Map*> borderingMaps{ GetBordering(cameraTransform) };
	for (const Map* const borderingMap : borderingMaps)
	{
		borderingMap->DrawUp();
	}
}

std::vector<Map*> Map::GetBordering(const Rectf& cameraTransform) const
{
	std::vector<Map*> borderingMaps{};
	bool south{ false };
	bool north{ false };
	if (m_South && cameraTransform.bottom < m_Boundaries.bottom)
	{
		borderingMaps.push_back(m_South);
		south = true;
	}
	else if (m_North && cameraTransform.bottom + cameraTransform.height > m_Boundaries.bottom + m_Boundaries.height)
	{
		borderingMaps.push_back(m_North);
		north = true;
	}

	if (m_West && cameraTransform.left < m_Boundaries.left)
	{
		borderingMaps.push_back(m_West);
		if (south)
		{
			Map* southWest{ m_South->GetWest() };
			if (southWest)
			{
				borderingMaps.push_back(southWest);
			}
			else
			{
				Map* westSouth{ m_West->GetSouth() };
				if (westSouth)
				{
					borderingMaps.push_back(westSouth);
				}
			}
		}
		else if (north)
		{
			Map* northWest{ m_North->GetWest() };
			if (northWest)
			{
				borderingMaps.push_back(northWest);
			}
			else
			{
				Map* westNorth{ m_West->GetNorth() };
				if (westNorth)
				{
					borderingMaps.push_back(westNorth);
				}
			}
		}
	}
	else if (m_East && cameraTransform.left + cameraTransform.width > m_Boundaries.left + m_Boundaries.width)
	{
		borderingMaps.push_back(m_East);
		if (south)
		{
			Map* southEast{ m_South->GetEast() };
			if (southEast)
			{
				borderingMaps.push_back(southEast);
			}
			else
			{
				Map* eastSouth{ m_East->GetSouth() };
				if (eastSouth)
				{
					borderingMaps.push_back(eastSouth);
				}
			}
		}
		else if (north)
		{
			Map* northEast{ m_North->GetEast() };
			if (northEast)
			{
				borderingMaps.push_back(northEast);
			}
			else
			{
				Map* eastNorth{ m_East->GetNorth() };
				if (eastNorth)
				{
					borderingMaps.push_back(eastNorth);
				}
			}
		}
	}
	return borderingMaps;
}

Map* Map::MapChange(const Rectf& moveRect)
{
	float centerX{ moveRect.left + moveRect.width / 2 };
	float centerY{ moveRect.bottom + moveRect.height / 2 };
	if (centerX < m_Boundaries.left)
	{
		if (GetWest())
		{
			return GetWest();
		}
		return this;
	}
	if (centerX > m_Boundaries.left + m_Boundaries.width)
	{
		if (GetEast())
		{
			return GetEast();
		}
		return this;
	}
	if (centerY < m_Boundaries.bottom)
	{
		if (GetSouth())
		{
			return GetSouth();
		}
		return this;
	}
	if (centerY > m_Boundaries.bottom + m_Boundaries.height)
	{
		if (GetNorth())
		{
			return GetNorth();
		}
		return this;
	}
	return this;
}

void Map::AddInteractable(Interactable* interactable)
{
	m_Interactables.push_back(interactable);
}

void Map::AddWarpPoint(WarpPoint* warpPoint)
{
	m_WarpPoints.push_back(warpPoint);
}

WarpPoint* Map::CheckWarp(const Rectf& actorShape)
{
	for (WarpPoint* warpPoint : m_WarpPoints)
	{
		if (warpPoint->Warp(actorShape))
		{
			return warpPoint;
		}
	}
	return nullptr;
}

void Map::AddStairs(Stairs* stairs)
{
	m_Stairs.push_back(stairs);
}

Stairs* Map::CheckStairs(const Rectf& actorShape)
{
	for (Stairs* stairs : m_Stairs)
	{
		if (stairs->TakeStairs(actorShape))
		{
			return stairs;
		}
	}
	return nullptr;
}

void Map::SetFloor(Floor* floor)
{
	m_Floor = floor;
}

void Map::SetCurrentFloor()
{
	m_Floor->SetCurrentFloor();
}

Floor* Map::GetFloor() const
{
	return m_Floor;
}

int Map::GetId() const
{
	return m_Id;
}

void Map::Reset()
{
	for (Interactable* interactable : m_Interactables)
	{
		interactable->Reset();
	}
}
