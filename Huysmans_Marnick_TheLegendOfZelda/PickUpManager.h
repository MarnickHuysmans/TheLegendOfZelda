#pragma once
#include <list>

class Player;
class PickUp;

class PickUpManager final
{
public:
	PickUpManager();
	~PickUpManager();

	PickUpManager(const PickUpManager& other) = delete;
	PickUpManager(PickUpManager&& other) noexcept = delete;
	PickUpManager& operator=(const PickUpManager& other) = delete;
	PickUpManager& operator=(PickUpManager&& other) noexcept = delete;

	void Update(float elapsedSec, Player& player);
	void Draw() const;
	void AddPickUp(PickUp* pickUp);

	void RemovePickUps();
private:
	std::list<PickUp*> m_PickUps;
};

