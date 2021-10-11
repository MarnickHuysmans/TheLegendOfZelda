#pragma once
#include "PickUp.h"
#include "StaticPickUp.h"

class Texture;
class Player;

class Rupee final : public PickUp, public StaticPickUp<Rupee>
{
public:
	enum class Color
	{
		green = 0,
		blue = 1,
		red = 2
	};

	Rupee(const Point2f& location, Color color);
	~Rupee() = default;

	Rupee(const Rupee& other) = delete;
	Rupee(Rupee&& other) noexcept = delete;
	Rupee& operator=(const Rupee& other) = delete;
	Rupee& operator=(Rupee&& other) noexcept = delete;

	void Draw() const override;
	bool Update(const float elapsedSec, Player& player) override;

	Rupee* Copy() override;

	bool Overlap(Player& player) override;
private:
	static int m_NrOfColors;

	Color m_Color;

	int GetValue() const override;
};
