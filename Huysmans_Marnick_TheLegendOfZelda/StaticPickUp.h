#pragma once
#include "Texture.h"
#include "SoundEffect.h"

template <typename T>
class StaticPickUp
{
public:
	static void SetSprite(Texture* sprite);
	static void SetNrOfFrames(int frames);
	static void SetSize(const Point2f& size);
	static void SetPickUpSound(SoundEffect* soundEffect);
protected:
	static Texture* m_Sprite;
	static float m_FramesPerSec;
	static int m_NrOfFrames;
	static float m_FadeTime;
	static float m_DestroyTime;
	static Point2f m_Size;
	static SoundEffect* m_PickUpSound;
};

template <typename T> Texture* StaticPickUp<T>::m_Sprite{ nullptr };
template <typename T> float StaticPickUp<T>::m_FramesPerSec{ 10.0f };
template <typename T> int StaticPickUp<T>::m_NrOfFrames{ 1 };
template <typename T> float StaticPickUp<T>::m_FadeTime{ 12.0f };
template <typename T> float StaticPickUp<T>::m_DestroyTime{ 3.0f };
template <typename T> Point2f StaticPickUp<T>::m_Size{ 1.0f, 1.0f };
template <typename T> SoundEffect* StaticPickUp<T>::m_PickUpSound{ nullptr };

template <typename T>
void StaticPickUp<T>::SetSprite(Texture* sprite)
{
	m_Sprite = sprite;
}

template <typename T>
void StaticPickUp<T>::SetNrOfFrames(int frames)
{
	m_NrOfFrames = frames;
}

template <typename T>
void StaticPickUp<T>::SetSize(const Point2f& size)
{
	m_Size = size;
}

template <typename T>
void StaticPickUp<T>::SetPickUpSound(SoundEffect* soundEffect)
{
	m_PickUpSound = soundEffect;
}
