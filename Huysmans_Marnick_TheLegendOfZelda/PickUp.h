#pragma once
class Player;

class PickUp
{
public:
	PickUp(const Rectf& hitBox);
	virtual ~PickUp() {}

	virtual void Draw() const = 0;
	virtual bool Update(const float elapsedSec, Player& player) = 0;
	virtual bool Overlap(Player& player);
	virtual PickUp* Copy() = 0;
protected:
	Rectf m_HitBox;
	float m_AnimTime;
	int m_AnimFrame;
	float m_FadeTimer;
	float m_DestroyTimer;
	bool m_Draw;
private:
	virtual int GetValue() const = 0;
};

