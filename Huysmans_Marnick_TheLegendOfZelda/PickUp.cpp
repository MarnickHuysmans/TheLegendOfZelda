#include "pch.h"
#include "PickUp.h"
#include "utils.h"
#include "Player.h"

PickUp::PickUp(const Rectf& hitBox)
	: m_HitBox(hitBox)
	, m_AnimTime(0.0f)
	, m_AnimFrame(0)
	, m_FadeTimer(0.0f)
	, m_DestroyTimer(0.0f)
	, m_Draw(true)
{
}

bool PickUp::Overlap(Player& player)
{
	if (utils::IsOverlapping(m_HitBox, player.GetWalkBox()))
	{
		return true;
	}
	return false;
}
