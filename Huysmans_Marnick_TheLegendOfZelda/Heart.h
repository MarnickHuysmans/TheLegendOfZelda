#pragma once
#include "PickUp.h"
#include "StaticPickUp.h"

class Heart final : public PickUp, public StaticPickUp<Heart>
{
public:
	Heart(const Point2f& location);
	~Heart() = default;

	Heart(const Heart& other) = delete;
	Heart(Heart&& other) noexcept = delete;
	Heart& operator=(const Heart& other) = delete;
	Heart& operator=(Heart&& other) noexcept = delete;


	void Draw() const override;
	bool Update(const float elapsedSec, Player& player) override;

	Heart* Copy() override;
	
	bool Overlap(Player& player) override;
private:
	int GetValue() const override;
	const int m_Value{ 2 };
};

