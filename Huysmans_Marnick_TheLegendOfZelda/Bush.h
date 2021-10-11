#pragma once
#include "Interactable.h"
#include "Vector2f.h"
#include "StaticInteractable.h"

class Bush final : public Interactable, public StaticInteractable<Bush>
{
public:
	Bush(const Rectf& boundaries, const bool ground);
	~Bush() = default;

	Bush(const Bush& other) = delete;
	Bush(Bush&& other) noexcept = delete;
	Bush& operator=(const Bush& other) = delete;
	Bush& operator=(Bush&& other) noexcept = delete;

	void Draw() const override;
	void SetPosition(Point2f& positionBottomCenter) override;
	
	bool SwordHit(const std::vector<Point2f>& hitBox, PickUpManager& pickUpManager) override;
	
	void Destroying() override;
private:
	void DropPickUp(PickUpManager& pickUpManager) override;
	static const float m_Chance;
	
	bool Flying(float elapsedSec, const Map* const map) override;
	void RandomPickUp(PickUpManager& pickUpManager);
};

