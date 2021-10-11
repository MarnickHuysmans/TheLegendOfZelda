#include "pch.h"
#include "PickUpManager.h"
#include "PickUp.h"

PickUpManager::PickUpManager()
{
}

PickUpManager::~PickUpManager()
{
	RemovePickUps();
}

void PickUpManager::Update(float elapsedSec, Player& player)
{
	auto it{ m_PickUps.begin() };
	while (it != m_PickUps.end())
	{
		if ((*it)->Update(elapsedSec, player))
		{
			delete* it;
			it = m_PickUps.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void PickUpManager::Draw() const
{
	for (PickUp* const pickUp : m_PickUps)
	{
		pickUp->Draw();
	}
}

void PickUpManager::AddPickUp(PickUp* pickUp)
{
	m_PickUps.push_back(pickUp);
}

void PickUpManager::RemovePickUps()
{
	auto it{ m_PickUps.begin() };
	for (; it != m_PickUps.end();)
	{
		delete* it;
		it = m_PickUps.erase(it);
	}
}
