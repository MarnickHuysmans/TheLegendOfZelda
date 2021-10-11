#pragma once
#include <vector>

class Texture;

template <typename T>
class StaticEnemy
{
public:
	static void SetNrOfFrames(std::vector<std::vector<int>>& nrOfFrames);
	static void SetSpriteTextures(std::vector<Texture*>& spriteTextures);
	static void SetSpriteOffsets(std::vector<Point2f>& spriteOffsets);
	static void SetFramesPerSecond(std::vector<float>& framesPerSecond);
	static void SetStateTimings(std::vector<float>& timings);
	static void SetHitSound(SoundEffect* soundEffect);
	static void SetEnemySound(SoundEffect* soundEffect);
	static void SetSize(Vector2f& size);
	static void SetHitBox(Rectf& hitBox);
	static Vector2f GetSize();
protected:
	static std::vector<std::vector<int>> m_NrOfFrames;
	static std::vector<Texture*> m_SpriteTextures;
	static std::vector<Point2f> m_SpriteOffsets;
	static std::vector<float> m_FramesPerSecond;
	static std::vector<float> m_StateTimings;
	static SoundEffect* m_HitSound;
	static SoundEffect* m_EnemySound;
	static Vector2f m_Size;
	static Rectf m_HitBox;

	static Rectf GetStaticHitBox();
};

template <typename T> std::vector<std::vector<int>> StaticEnemy<T>::m_NrOfFrames{};
template <typename T> std::vector<Texture*> StaticEnemy<T>::m_SpriteTextures{};
template <typename T> std::vector<Point2f> StaticEnemy<T>::m_SpriteOffsets{};
template <typename T> std::vector<float> StaticEnemy<T>::m_FramesPerSecond{};
template <typename T> std::vector<float> StaticEnemy<T>::m_StateTimings{};
template <typename T> SoundEffect* StaticEnemy<T>::m_HitSound{ nullptr };
template <typename T> SoundEffect* StaticEnemy<T>::m_EnemySound{ nullptr };
template <typename T> Vector2f StaticEnemy<T>::m_Size{};
template <typename T> Rectf StaticEnemy<T>::m_HitBox{};

template <typename T>
void StaticEnemy<T>::SetNrOfFrames(std::vector<std::vector<int>>& nrOfFrames)
{
	m_NrOfFrames = std::move(nrOfFrames);
}

template <typename T>
void StaticEnemy<T>::SetSpriteTextures(std::vector<Texture*>& spriteTextures)
{
	m_SpriteTextures = std::move(spriteTextures);
}

template <typename T>
void StaticEnemy<T>::SetSpriteOffsets(std::vector<Point2f>& spriteOffsets)
{
	m_SpriteOffsets = std::move(spriteOffsets);
}

template <typename T>
void StaticEnemy<T>::SetFramesPerSecond(std::vector<float>& framesPerSecond)
{
	m_FramesPerSecond = std::move(framesPerSecond);
}

template <typename T>
void StaticEnemy<T>::SetStateTimings(std::vector<float>& timings)
{
	m_StateTimings = std::move(timings);
}

template <typename T>
void StaticEnemy<T>::SetHitSound(SoundEffect* soundEffect)
{
	m_HitSound = soundEffect;
}

template <typename T>
void StaticEnemy<T>::SetEnemySound(SoundEffect* soundEffect)
{
	m_EnemySound = soundEffect;
}

template <typename T>
void StaticEnemy<T>::SetSize(Vector2f& size)
{
	m_Size = size;
}

template <typename T>
void StaticEnemy<T>::SetHitBox(Rectf& hitBox)
{
	m_HitBox = hitBox;
}

template <typename T>
Vector2f StaticEnemy<T>::GetSize()
{
	return m_Size;
}

template <typename T>
Rectf StaticEnemy<T>::GetStaticHitBox()
{
	return m_HitBox;
}
