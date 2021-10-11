#pragma once
#include "Interactable.h"
#include "StaticInteractable.h"
#include "PickUp.h"

class Pot final : public Interactable, public StaticInteractable<Pot>
{
public:
	Pot(const Rectf& boundaries, PickUp* pickUp, const bool ground);
	~Pot();

	Pot(const Pot& other) = delete;
	Pot(Pot&& other) noexcept = delete;
	Pot& operator=(const Pot& other) = delete;
	Pot& operator=(Pot&& other) noexcept = delete;

	void Draw() const override;
	void SetPosition(Point2f& positionBottomCenter) override;
	
	void Destroying() override;
private:
	PickUp* m_PickUp;
	
	bool Flying(float elapsedSec, const Map* map) override;
	void DropPickUp(PickUpManager& pickUpManager) override;
};

