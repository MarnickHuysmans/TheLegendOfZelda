#pragma once
#include <vector>
#include "Level.h"
#include "SwordSoldier.h"
#include "PickUp.h"

class SoundManager;
class TextureManager;

class LevelInitializer final
{
public:
	static Map* InitLevels(std::vector<Level*>& levels, TextureManager& textureManager, SoundManager& soundManager);

private:
	struct Connection
	{
		Connection(Map* pMap)
			: map(pMap)
			, north(-1)
			, east(-1)
			, south(-1)
			, west(-1)
		{}

		Map* map;
		int north;
		int east;
		int south;
		int west;
	};

	struct Warp
	{
		Warp(Map* pMap, Rectf& rectf)
			: home(pMap)
			, level(-1)
			, floor(-1)
			, map(-1)
			, boundaries(rectf)
			, ground(true)
			, direction(Direction::north)
		{}
		Map* home;
		int level;
		int floor;
		int map;
		Rectf boundaries;
		Point2f warpLocation;
		bool ground;
		Direction direction;
	};

	static std::string m_ResourcesFolder;

	static void InitLevel(const std::string& folder, const std::string& levelName, std::vector<Level*>& levels, TextureManager& textureManager, std::vector<Connection>& connections, std::vector<Warp>& warps, SoundManager& soundManager);
	static void InitFloor(const std::string& folder, const std::string& floorName, Level* const level, TextureManager& textureManager, std::vector<Connection>& connections, std::vector<Warp>& warps);
	static Map* InitMap(const std::string& folder, const std::string& mapName, const Vector2f& location, Floor* const floor, TextureManager& textureManager, std::vector<Warp>& warps);

	static void AddInteractables(const std::string& folder, const Vector2f& location, Map* map);
	static void AddBushes(const std::string& folder, const Vector2f& location, Map* map);
	static void AddPots(const std::string& folder, const Vector2f& location, Map* map);

	static void AddConnections(std::vector<Connection>& connections, const std::string& str, Map* map);
	static void AddConnection(Connection& connection, std::string direction, std::string map);
	static void CreateConnections(std::vector<Connection>& connections);

	static void AddWarpPoints(std::vector<Warp>& warps, const std::string& folder, Map* map, const Vector2f& location);
	static void AddWarpPoint(std::vector<Warp>& warps, const std::string& str, Map* map, const Vector2f& location);
	static void WarpTo(Warp& warp, const std::string& str);
	static void CreateWarpPoints(std::vector<Warp>& warps, std::vector<Level*>& levels);
	static void CreateWarpPoint(Warp& warp, std::vector<Level*>& levels);

	static void AddStairs(const std::string& folder, Map* map, const Vector2f& location);
	static void AddStair(const std::string& str, Map* map, const Vector2f& location);

	static void AddEnemies(const std::string& folder, Map* map, const Vector2f& location);
	static void AddSwordSoldiers(const std::string& folder, Map* map, const Vector2f& location);
	static void AddSwordSoldier(const std::string& str, Map* map, const Vector2f& offset);
	static SwordSoldier::Color GetSwordSoldierColor(std::string str);
	static void AddRats(const std::string& folder, Map* map, const Vector2f& location);
	static void AddRat(const std::string& str, Map* map, const Vector2f& offset);

	static PickUp* CreatePickUp(const std::string& str, const Point2f& location);
};
