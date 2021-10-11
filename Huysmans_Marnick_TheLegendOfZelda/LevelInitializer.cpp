#include "pch.h"
#include "LevelInitializer.h"
#include <fstream>
#include <sstream>
#include "TextureManager.h"
#include "SVGParser.h"
#include "Bush.h"
#include "Rat.h"
#include "Pot.h"
#include "utils.h"
#include "Heart.h"
#include "SoundManager.h"

std::string LevelInitializer::m_ResourcesFolder{ "Resources/" };

Map* LevelInitializer::InitLevels(std::vector<Level*>& levels, TextureManager& textureManager, SoundManager& soundManager)
{
	std::vector<Connection> connections{};
	std::vector<Warp> warps{};

	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << "Level.txt";
	std::ifstream levelStream{ stringStream.str() };
	if (levelStream)
	{
		std::string level{};
		while (std::getline(levelStream, level))
		{
			InitLevel(m_ResourcesFolder, utils::Trim(level), levels, textureManager, connections, warps, soundManager);
		}
	}
	levelStream.close();

	CreateConnections(connections);

	CreateWarpPoints(warps, levels);

	stringStream.clear();
	stringStream.str("");
	stringStream << m_ResourcesFolder << "Start.txt";
	std::vector<int> index{};
	std::ifstream startStream{ stringStream.str() };
	if (startStream)
	{
		std::string line{};
		while (std::getline(startStream, line))
		{
			index.push_back(std::stoi(utils::Trim(line)));
		}
	}
	while (index.size() < 3)
	{
		index.push_back(0);
	}
	return levels[index[0]]->GetFloor(index[1])->GetMap(index[2]);
}

void LevelInitializer::InitLevel(const std::string& folder, const std::string& levelName, std::vector<Level*>& levels, TextureManager& textureManager, std::vector<Connection>& connections, std::vector<Warp>& warps, SoundManager& soundManager)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << levelName << "/";
	std::string levelFolder{ stringStream.str() };
	stringStream << "Start.mp3";
	SoundStream* start{ soundManager.CreateSoundStream(stringStream.str()) };
	stringStream.str("");
	stringStream << levelFolder << "Loop.mp3";
	SoundStream* loop{ soundManager.CreateSoundStream(stringStream.str()) };
	Level* level{ new Level{ static_cast<const int>(levels.size()), start, loop } };
	levels.push_back(level);

	stringStream.str("");
	stringStream << levelFolder << "Floor.txt";
	std::ifstream floorStream{ stringStream.str() };
	if (floorStream)
	{
		std::string floor{};
		while (std::getline(floorStream, floor))
		{
			InitFloor(levelFolder, utils::Trim(floor), level, textureManager, connections, warps);
		}
	}
}

void LevelInitializer::InitFloor(const std::string& folder, const std::string& floorName, Level* const level, TextureManager& textureManager, std::vector<Connection>& connections, std::vector<Warp>& warps)
{
	Floor* floor{ new Floor{std::stoi(floorName)} };
	level->AddFloor(floor);

	std::stringstream stringStream{ "" };
	stringStream << folder << floorName << "/";
	std::string floorFolder{ stringStream.str() };
	stringStream << "Map.txt";
	std::ifstream mapStream{ stringStream.str() };
	if (mapStream)
	{
		std::string map{};
		while (std::getline(mapStream, map))
		{
			stringStream.clear();
			stringStream.str("");
			stringStream << map;
			std::string mapId{};
			std::getline(stringStream, mapId, ':');
			std::string mapLocation{};
			std::getline(stringStream, mapLocation, ';');
			Map* pMap = InitMap(floorFolder, mapId, utils::GetLocation(mapLocation), floor, textureManager, warps);
			if (!stringStream.eof())
			{
				std::string line{};
				std::getline(stringStream, line);
				AddConnections(connections, line, pMap);
			}
		}
	}
}

Map* LevelInitializer::InitMap(const std::string& folder, const std::string& mapName, const Vector2f& location, Floor* const floor, TextureManager& textureManager, std::vector<Warp>& warps)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << mapName << "/";
	std::string mapFolder{ stringStream.str() };

	stringStream.clear();
	stringStream.str("");
	stringStream << mapFolder << mapName << ".svg";
	std::vector<std::vector<Point2f>> vertices{};
	std::ifstream file{ stringStream.str() };
	if (file.good())
	{
		SVGParser::GetVerticesFromSvgFile(stringStream.str(), vertices);
		for (std::vector<Point2f>& hitBox : vertices)
		{
			for (Point2f& vertex : hitBox)
			{
				vertex += location;
			}
		}
	}
	file.close();

	stringStream.clear();
	stringStream.str("");
	stringStream << mapFolder << mapName << "Up.svg";
	std::vector<std::vector<Point2f>> verticesUp{};
	file.open(stringStream.str());
	if (file.good())
	{
		SVGParser::GetVerticesFromSvgFile(stringStream.str(), verticesUp);
		for (std::vector<Point2f>& hitBox : verticesUp)
		{
			for (Point2f& vertex : hitBox)
			{
				vertex += location;
			}
		}
	}
	file.close();

	stringStream.clear();
	stringStream.str("");
	stringStream << mapFolder << mapName << ".png";
	Texture* backGround{ textureManager.CreateTexture(stringStream.str()) };
	Rectf boundaries{};
	if (backGround)
	{
		boundaries.width = backGround->GetWidth();
		boundaries.height = backGround->GetHeight();
		boundaries += location;
	}

	stringStream.clear();
	stringStream.str("");
	stringStream << mapFolder << mapName << "Top.png";
	Texture* top{ textureManager.CreateTexture(stringStream.str()) };

	stringStream.clear();
	stringStream.str("");
	stringStream << mapFolder << mapName << "Up.png";
	Texture* up{ textureManager.CreateTexture(stringStream.str()) };

	stringStream.clear();
	stringStream.str("");
	stringStream << mapFolder << mapName << "Animation0.png";
	Texture* animation0{ textureManager.CreateTexture(stringStream.str()) };
	stringStream.clear();
	stringStream.str("");
	stringStream << mapFolder << mapName << "Animation1.png";
	Texture* animation1{ textureManager.CreateTexture(stringStream.str()) };
	std::vector<Texture*> animations{};
	if (animation0 && animation1)
	{
		animations.push_back(animation0);
		animations.push_back(animation1);
	}
	else
	{
		if (!animation0)
		{
			delete animation0;
		}
		if (!animation1)
		{
			delete animation1;
		}
	}

	Map* map{ new Map{std::stoi(mapName), vertices, verticesUp, backGround, top, up, animations, boundaries} };
	floor->AddMap(map);

	AddInteractables(mapFolder, location, map);

	AddWarpPoints(warps, mapFolder, map, location);

	AddStairs(mapFolder, map, location);

	AddEnemies(mapFolder, map, location);

	return map;
}

void LevelInitializer::AddInteractables(const std::string& folder, const Vector2f& location, Map* map)
{
	AddBushes(folder, location, map);
	AddPots(folder, location, map);
}

void LevelInitializer::AddBushes(const std::string& folder, const Vector2f& location, Map* map)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << "Bush.txt";
	std::ifstream bushStream{ stringStream.str() };
	if (bushStream)
	{
		Rectf rect{ Bush::GetInteractableTextureRect() };
		std::string line{};
		while (std::getline(bushStream, line))
		{
			stringStream.clear();
			stringStream.str("");
			stringStream << utils::Trim(line);
			std::getline(stringStream, line, ':');
			std::getline(stringStream, line, ';');
			Vector2f bushOffset{ utils::GetLocation(line) };
			Rectf boundaries{ rect + bushOffset + location };
			std::getline(stringStream, line);
			bool ground{ utils::GetBool(line) };
			Bush* bush{ new Bush{boundaries, ground} };
			map->AddInteractable(bush);
		}
	}
}

void LevelInitializer::AddPots(const std::string& folder, const Vector2f& location, Map* map)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << "Pot.txt";
	std::ifstream potStream{ stringStream.str() };
	if (potStream)
	{
		Rectf rect{ Pot::GetInteractableTextureRect() };
		std::string line{};
		while (std::getline(potStream, line))
		{
			stringStream.clear();
			stringStream.str("");
			stringStream << utils::Trim(line);
			std::getline(stringStream, line, ':');
			std::getline(stringStream, line, ';');
			Vector2f bushOffset{ utils::GetLocation(line) };
			Rectf boundaries{ rect + bushOffset + location };
			std::getline(stringStream, line, ';');

			Point2f location{ boundaries.left, boundaries.bottom };
			location += Pot::GetPickUpOffset();
			PickUp* pickUp{ CreatePickUp(line, location) };
			std::getline(stringStream, line);
			bool ground{ utils::GetBool(line) };
			Pot* pot{ new Pot{boundaries, pickUp, ground} };
			map->AddInteractable(pot);
		}
	}
}

void LevelInitializer::AddConnections(std::vector<Connection>& connections, const std::string& str, Map* map)
{
	Connection connection{ map };
	std::stringstream stringStream{ "" };
	stringStream << str;
	std::string line{};
	while (std::getline(stringStream, line, ';'))
	{
		std::stringstream stream{ "" };
		stream << line;
		std::string direction{};
		std::getline(stream, direction, ':');
		std::string mapId{};
		std::getline(stream, mapId);
		AddConnection(connection, utils::Trim(direction), utils::Trim(mapId));
	}

	connections.push_back(connection);
}

void LevelInitializer::AddConnection(Connection& connection, std::string direction, std::string map)
{
	if (direction == "N")
	{
		connection.north = std::stoi(map);
	}
	else if (direction == "E")
	{
		connection.east = std::stoi(map);
	}
	else if (direction == "S")
	{
		connection.south = std::stoi(map);
	}
	else if (direction == "W")
	{
		connection.west = std::stoi(map);
	}
}

void LevelInitializer::CreateConnections(std::vector<Connection>& connections)
{
	for (Connection& connection : connections)
	{
		Floor* floor{ connection.map->GetFloor() };
		connection.map->SetNorth(floor->GetMapById(connection.north));
		connection.map->SetEast(floor->GetMapById(connection.east));
		connection.map->SetSouth(floor->GetMapById(connection.south));
		connection.map->SetWest(floor->GetMapById(connection.west));
	}
}

void LevelInitializer::AddWarpPoints(std::vector<Warp>& warps, const std::string& folder, Map* map, const Vector2f& location)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << "WarpPoint.txt";
	std::ifstream warpPointStream{ stringStream.str() };
	if (warpPointStream)
	{
		std::string line{};
		while (std::getline(warpPointStream, line))
		{
			AddWarpPoint(warps, utils::Trim(line), map, location);
		}
	}
}

void LevelInitializer::AddWarpPoint(std::vector<Warp>& warps, const std::string& str, Map* map, const Vector2f& location)
{
	std::stringstream stringStream{ "" };
	stringStream << str;
	std::string line{};
	std::getline(stringStream, line, ':');
	std::getline(stringStream, line, ';');
	Rectf boundaries{ utils::GetBoundaries(line) };
	boundaries += location;
	Warp warp{ map, boundaries };

	std::getline(stringStream, line, ';');
	while (line.substr(0, 1) != "(")
	{
		WarpTo(warp, line);
		std::getline(stringStream, line, ';');
	}
	warp.warpLocation = utils::GetLocation(line).ToPoint2f();
	std::getline(stringStream, line, ';');
	warp.ground = utils::GetBool(line);
	std::getline(stringStream, line);
	warp.direction = utils::GetDirection(line);
	warps.push_back(warp);
}

void LevelInitializer::WarpTo(Warp& warp, const std::string& str)
{
	std::stringstream stringStream{ "" };
	stringStream << utils::Trim(str);
	std::string line{};
	std::getline(stringStream, line, ':');
	std::string index{};
	std::getline(stringStream, index);
	line = utils::Trim(line);
	if (line == "L")
	{
		warp.level = std::stoi(utils::Trim(index));
	}
	else if (line == "F")
	{
		warp.floor = std::stoi(utils::Trim(index));
	}
	else if (line == "M")
	{
		warp.map = std::stoi(utils::Trim(index));
	}
}

void LevelInitializer::CreateWarpPoints(std::vector<Warp>& warps, std::vector<Level*>& levels)
{
	for (Warp& warp : warps)
	{
		CreateWarpPoint(warp, levels);
	}
}

void LevelInitializer::CreateWarpPoint(Warp& warp, std::vector<Level*>& levels)
{

	Floor* floor{};
	if (warp.floor > -1)
	{
		Level* level{};
		if (warp.level > -1)
		{
			level = levels[warp.level];
		}
		else
		{
			level = warp.home->GetFloor()->GetLevel();
		}
		floor = level->GetFloor(warp.floor);
	}
	else
	{
		floor = warp.home->GetFloor();
	}
	Map* map{ floor->GetMapById(warp.map) };
	Rectf mapBoundaries{ map->GetBoundaries() };
	Vector2f offset{ mapBoundaries.left, mapBoundaries.bottom };
	warp.warpLocation += offset;
	WarpPoint* warpPoint{ new WarpPoint{warp.boundaries, map, warp.warpLocation, warp.ground, warp.direction} };
	warp.home->AddWarpPoint(warpPoint);
}

void LevelInitializer::AddStairs(const std::string& folder, Map* map, const Vector2f& location)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << "Stairs.txt";
	std::ifstream stairsStream{ stringStream.str() };
	if (stairsStream)
	{
		std::string line{};
		while (std::getline(stairsStream, line))
		{
			AddStair(utils::Trim(line), map, location);
		}
	}
}

void LevelInitializer::AddStair(const std::string& str, Map* map, const Vector2f& location)
{
	std::stringstream stringStream{ "" };
	stringStream << str;
	std::string line{};
	std::getline(stringStream, line, ':');
	std::getline(stringStream, line, ';');
	Rectf boundaries{ utils::GetBoundaries(line) };
	boundaries += location;
	std::getline(stringStream, line);
	bool ground{ utils::GetBool(line) };
	Stairs* stairs{ new Stairs{boundaries, ground} };
	map->AddStairs(stairs);
}

void LevelInitializer::AddEnemies(const std::string& folder, Map* map, const Vector2f& location)
{
	AddSwordSoldiers(folder, map, location);

	AddRats(folder, map, location);
}

void LevelInitializer::AddSwordSoldiers(const std::string& folder, Map* map, const Vector2f& location)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << "SwordSoldier.txt";
	std::ifstream stream{ stringStream.str() };
	if (stream)
	{
		std::string line{};
		while (std::getline(stream, line))
		{
			AddSwordSoldier(line, map, location);
		}
	}
}

void LevelInitializer::AddSwordSoldier(const std::string& str, Map* map, const Vector2f& offset)
{
	std::stringstream stringStream{ "" };
	stringStream << str;
	std::string line{};
	std::getline(stringStream, line, ':');
	std::getline(stringStream, line, ';');
	Vector2f location{ utils::GetLocation(line) };
	Vector2f size{ SwordSoldier::GetSize() };
	Rectf walkBox{ location.x, location.y, size.x, size.y };
	walkBox += offset;
	std::getline(stringStream, line, ';');
	SwordSoldier::Color color{ GetSwordSoldierColor(line) };
	std::getline(stringStream, line);
	bool ground{ utils::GetBool(line) };
	SwordSoldier* swordSoldier{ new SwordSoldier{walkBox, map, color, ground} };
	map->GetFloor()->AddEnemy(swordSoldier);
}

SwordSoldier::Color LevelInitializer::GetSwordSoldierColor(std::string str)
{
	if (str == "green")
	{
		return SwordSoldier::Color::green;
	}
	if (str == "blue")
	{
		return SwordSoldier::Color::blue;
	}
	if (str == "red")
	{
		return SwordSoldier::Color::red;
	}
	return SwordSoldier::Color::green;
}

void LevelInitializer::AddRats(const std::string& folder, Map* map, const Vector2f& location)
{
	std::stringstream stringStream{ "" };
	stringStream << folder << "Rat.txt";
	std::ifstream stream{ stringStream.str() };
	if (stream)
	{
		std::string line{};
		while (std::getline(stream, line))
		{
			AddRat(line, map, location);
		}
	}
}

void LevelInitializer::AddRat(const std::string& str, Map* map, const Vector2f& offset)
{
	std::stringstream stringStream{ "" };
	stringStream << str;
	std::string line{};
	std::getline(stringStream, line, ':');
	std::getline(stringStream, line, ';');
	Vector2f location{ utils::GetLocation(line) };
	Vector2f size{ Rat::GetSize() };
	Rectf walkBox{ location.x, location.y, size.x, size.y };
	walkBox += offset;
	std::getline(stringStream, line);
	bool ground{ utils::GetBool(line) };
	Rat* rat{ new Rat{walkBox, map, ground} };
	map->GetFloor()->AddEnemy(rat);
}

PickUp* LevelInitializer::CreatePickUp(const std::string& str, const Point2f& location)
{
	std::stringstream stringStream{ "" };
	stringStream << utils::Trim(str);
	std::string line{};
	std::getline(stringStream, line, ':');
	if (line == "heart")
	{
		return new Heart{ location };
	}
	if (line == "rupee")
	{
		std::getline(stringStream, line);
		return new Rupee{ location, utils::GetRupeeColor(line) };
	}
	return nullptr;
}