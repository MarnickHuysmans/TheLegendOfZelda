#include "pch.h"
#include "StaticInitializer.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include <sstream>
#include "Bush.h"
#include "Rupee.h"
#include "SwordSoldier.h"
#include "Rat.h"
#include <fstream>
#include "Heart.h"
#include "Pot.h"
#include "utils.h"

std::string StaticInitializer::m_ResourcesFolder{ "Resources/" };

void StaticInitializer::InitStatics(TextureManager& textureManager, SoundManager& soundManager)
{
	InitInteractables(textureManager, soundManager);
	InitPickUps(textureManager, soundManager);
	InitEnemies(textureManager, soundManager);
}

void StaticInitializer::InitInteractables(TextureManager& textureManager, SoundManager& soundManager)
{
	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << "Shared/Shadow.png";
	Texture* shadow{ textureManager.CreateTexture(stringStream.str()) };
	InitBush(textureManager, shadow, soundManager);
	InitPot(textureManager, shadow, soundManager);
}

void StaticInitializer::InitBush(TextureManager& textureManager, Texture* shadow, SoundManager& soundManager)
{
	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << "Bush/Bush.png";
	Texture* bush{ textureManager.CreateTexture(stringStream.str()) };
	Bush::SetInteractableTexture(bush);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "Bush/Destroy.png";
	Texture* destroy{ textureManager.CreateTexture(stringStream.str()) };
	Bush::SetDestroyTexture(destroy);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "Bush/Destroy.wav";
	SoundEffect* soundEffect{ soundManager.CreateSoundEffect(stringStream.str()) };
	Bush::SetDestroySound(soundEffect);

	Bush::SetPickUpOffset(Vector2f{ 4.0f, 2.0f });

	Bush::SetShadowTexture(shadow);
}

void StaticInitializer::InitPot(TextureManager& textureManager, Texture* shadow, SoundManager& soundManager)
{
	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << "Pot/Pot.png";
	Texture* pot{ textureManager.CreateTexture(stringStream.str()) };
	Pot::SetInteractableTexture(pot);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "Pot/Destroy.png";
	Texture* destroy{ textureManager.CreateTexture(stringStream.str()) };
	Pot::SetDestroyTexture(destroy);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "Pot/Destroy.wav";
	SoundEffect* soundEffect{ soundManager.CreateSoundEffect(stringStream.str()) };
	Pot::SetDestroySound(soundEffect);

	Pot::SetPickUpOffset(Vector2f{ 2.0f, 2.0f });

	Pot::SetShadowTexture(shadow);
}

void StaticInitializer::InitPickUps(TextureManager& textureManager, SoundManager& soundManager)
{
	InitRupee(textureManager, soundManager);

	InitHeart(textureManager, soundManager);
}

void StaticInitializer::InitRupee(TextureManager& textureManager, SoundManager& soundManager)
{
	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << "PickUp/Rupee.png";
	Texture* sprite{ textureManager.CreateTexture(stringStream.str()) };
	Rupee::SetSprite(sprite);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "PickUp/Rupee.wav";
	SoundEffect* soundEffect{ soundManager.CreateSoundEffect(stringStream.str()) };
	Rupee::SetPickUpSound(soundEffect);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "PickUp/Rupee.txt";
	std::ifstream stream{ stringStream.str() };
	if (stream)
	{
		std::string line{};
		std::getline(stream, line, ';');
		Point2f size{ utils::GetLocation(line).ToPoint2f() };
		Rupee::SetSize(size);
		std::getline(stream, line);
		Rupee::SetNrOfFrames(std::stoi(line));
	}
}

void StaticInitializer::InitHeart(TextureManager& textureManager, SoundManager& soundManager)
{
	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << "PickUp/Heart.png";
	Texture* sprite{ textureManager.CreateTexture(stringStream.str()) };
	Heart::SetSprite(sprite);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "PickUp/Heart.wav";
	SoundEffect* soundEffect{ soundManager.CreateSoundEffect(stringStream.str()) };
	Heart::SetPickUpSound(soundEffect);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "PickUp/Heart.txt";
	std::ifstream stream{ stringStream.str() };
	if (stream)
	{
		std::string line{};
		std::getline(stream, line);
		Point2f size{ utils::GetLocation(line).ToPoint2f() };
		Heart::SetSize(size);
	}
}

void StaticInitializer::InitEnemies(TextureManager& textureManager, SoundManager& soundManager)
{
	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << "Shared/EnemyDeath.png";
	Texture* death{ textureManager.CreateTexture(stringStream.str()) };

	stringStream.str("");
	stringStream << m_ResourcesFolder << "Shared/Death.wav";
	SoundEffect* deathSound{ soundManager.CreateSoundEffect(stringStream.str()) };
	Enemy::SetDeathSound(deathSound);

	stringStream.str("");
	stringStream << m_ResourcesFolder << "Shared/Hit.wav";
	SoundEffect* hitSound{ soundManager.CreateSoundEffect(stringStream.str()) };

	stringStream.str("");
	stringStream << m_ResourcesFolder << "Shared/HitSmall.wav";
	SoundEffect* hitSmallSound{ soundManager.CreateSoundEffect(stringStream.str()) };

	InitSwordSoldier(textureManager, death, hitSound, soundManager);
	InitRat(textureManager, death, hitSmallSound, soundManager);
}

void StaticInitializer::InitSwordSoldier(TextureManager& textureManager, Texture* death, SoundEffect* soundEffect, SoundManager& soundManager)
{
	SwordSoldier::SetHitSound(soundEffect);

	std::string folder{ "SwordSoldier/" };

	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << folder << "SwordSoldier.wav";
	SoundEffect* enemySound{ soundManager.CreateSoundEffect(stringStream.str()) };
	SwordSoldier::SetEnemySound(enemySound);

	std::vector<std::vector<int>> nrOfFrames{};
	std::vector<Texture*> spriteTextures{};
	std::vector<Point2f> spriteOffsets{};
	std::vector<Point2f> weaponOffsets{};
	float weaponLength{ 16 };
	float weaponWidth{ 5 };
	std::vector<float> timings{};

	std::vector<int> waiting{ 3,3,3,3,3 };
	std::vector<int> moving{ 4,4,4,4,4 };
	std::vector<int> damage{ 3,3,3,3,3 };
	std::vector<int> deathFrames{ 7,7,7,7,7 };
	nrOfFrames.push_back(waiting);
	nrOfFrames.push_back(moving);
	nrOfFrames.push_back(damage);
	nrOfFrames.push_back(deathFrames);
	SwordSoldier::SetNrOfFrames(nrOfFrames);

	std::vector<float> framesPerSecond{ 10.0f, 10.0f, 60.0f, 7.0f };
	SwordSoldier::SetFramesPerSecond(framesPerSecond);

	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "Waiting.png";
	Texture* waitingGreen{ textureManager.CreateTexture(stringStream.str()) };
	spriteTextures.push_back(waitingGreen);
	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "Moving.png";
	Texture* movingGreen{ textureManager.CreateTexture(stringStream.str()) };
	spriteTextures.push_back(movingGreen);
	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "Damage.png";
	Texture* damageGreen{ textureManager.CreateTexture(stringStream.str()) };
	spriteTextures.push_back(damageGreen);
	spriteTextures.push_back(death);
	SwordSoldier::SetSpriteTextures(spriteTextures);

	Point2f waitingOffset{ 14.0f, 9.0f };
	Point2f movingOffset{ 17.0f, 13.0f };
	Point2f damageOffset{ 14.0f, 9.0f };
	Point2f deathOffset{ 6.0f, -2.0f };
	spriteOffsets.push_back(waitingOffset);
	spriteOffsets.push_back(movingOffset);
	spriteOffsets.push_back(damageOffset);
	spriteOffsets.push_back(deathOffset);
	SwordSoldier::SetSpriteOffsets(spriteOffsets);

	Point2f south{ -5.0f, -12.0f };
	Point2f west{ -17.0f, 6.0f };
	Point2f north{ 11.0f, 16.0f };
	Point2f east{ 13.0f, 6.0f };
	weaponOffsets.push_back(south);
	weaponOffsets.push_back(west);
	weaponOffsets.push_back(north);
	weaponOffsets.push_back(east);
	SwordSoldier::SetWeaponOffsets(weaponOffsets);

	SwordSoldier::SetWeaponSize(weaponLength, weaponWidth);

	float waitingTime{ 1.5f };
	float movingTime{ 4.0f };
	float damageTime{ 0.5f };
	timings.push_back(waitingTime);
	timings.push_back(movingTime);
	timings.push_back(damageTime);
	SwordSoldier::SetStateTimings(timings);

	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "SwordSoldier.txt";
	std::ifstream stream{ stringStream.str() };
	if (stream)
	{
		std::string line{};
		std::getline(stream, line, ';');
		Vector2f size{ utils::GetLocation(line) };
		SwordSoldier::SetSize(size);
		std::getline(stream, line);
		Rectf hitBox{ utils::GetBoundaries(line) };
		SwordSoldier::SetHitBox(hitBox);
	}
}

void StaticInitializer::InitRat(TextureManager& textureManager, Texture* death, SoundEffect* soundEffect, SoundManager& soundManager)
{
	Rat::SetHitSound(soundEffect);

	std::string folder{ "Rat/" };

	std::stringstream stringStream{ "" };
	stringStream << m_ResourcesFolder << folder << "Rat.wav";
	SoundEffect* enemySound{ soundManager.CreateSoundEffect(stringStream.str()) };
	Rat::SetEnemySound(enemySound);

	std::vector<std::vector<int>> nrOfFrames{};
	std::vector<Texture*> spriteTextures{};
	std::vector<Point2f> spriteOffsets{};
	std::vector<float> timings{};

	std::vector<int> waiting{ 2,2,2,2,2 };
	std::vector<int> moving{ 2,2,2,2,2 };
	std::vector<int> damage{ 1,1,1,1,1 };
	std::vector<int> deathFrames{ 7,7,7,7,7 };
	nrOfFrames.push_back(waiting);
	nrOfFrames.push_back(moving);
	nrOfFrames.push_back(damage);
	nrOfFrames.push_back(deathFrames);
	Rat::SetNrOfFrames(nrOfFrames);

	std::vector<float> framesPerSecond{ 10.0f, 10.0f, 60.0f, 7.0f };
	Rat::SetFramesPerSecond(framesPerSecond);

	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "Waiting.png";
	Texture* waitingTexture{ textureManager.CreateTexture(stringStream.str()) };
	spriteTextures.push_back(waitingTexture);
	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "Moving.png";
	Texture* movingTexture{ textureManager.CreateTexture(stringStream.str()) };
	spriteTextures.push_back(movingTexture);
	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "Damage.png";
	Texture* damageTexture{ textureManager.CreateTexture(stringStream.str()) };
	spriteTextures.push_back(damageTexture);
	spriteTextures.push_back(death);
	Rat::SetSpriteTextures(spriteTextures);

	Point2f waitingOffset{ 0.0f, 0.0f };
	Point2f movingOffset{ 0.0f, 0.0f };
	Point2f damageOffset{ 0.0f, 0.0f };
	Point2f deathOffset{ 4.0f, 3.0f };
	spriteOffsets.push_back(waitingOffset);
	spriteOffsets.push_back(movingOffset);
	spriteOffsets.push_back(damageOffset);
	spriteOffsets.push_back(deathOffset);
	Rat::SetSpriteOffsets(spriteOffsets);

	float waitingTime{ 1.5f };
	float movingTime{ 4.0f };
	float damageTime{ 0.5f };
	timings.push_back(waitingTime);
	timings.push_back(movingTime);
	timings.push_back(damageTime);
	Rat::SetStateTimings(timings);

	stringStream.str("");
	stringStream << m_ResourcesFolder << folder << "Rat.txt";
	std::ifstream stream{ stringStream.str() };
	if (stream)
	{
		std::string line{};
		std::getline(stream, line, ';');
		Vector2f size{ utils::GetLocation(line) };
		Rat::SetSize(size);
		std::getline(stream, line);
		Rectf hitBox{ utils::GetBoundaries(line) };
		Rat::SetHitBox(hitBox);
	}
}
