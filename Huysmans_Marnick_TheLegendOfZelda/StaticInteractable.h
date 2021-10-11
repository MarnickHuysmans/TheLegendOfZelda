#pragma once

class Texture;
class SoundEffect;

template <typename T>
class StaticInteractable
{
public:
	static void SetInteractableTexture(Texture* texture);
	static void SetDestroyTexture(Texture* texture);
	static void SetShadowTexture(Texture* texture);
	static Rectf GetInteractableTextureRect();
	static void SetPickUpOffset(const Vector2f& offset);
	static Vector2f GetPickUpOffset();
	static void SetDestroySound(SoundEffect* soundEffect);
protected:
	static Texture* m_InteractableTexture;
	static Texture* m_Destroy;
	static Texture* m_Shadow;
	static Vector2f m_PickUpOffset;
	static SoundEffect* m_DestroySound;
};

template <typename T> Texture* StaticInteractable<T>::m_InteractableTexture{ nullptr };
template <typename T> Texture* StaticInteractable<T>::m_Destroy{ nullptr };
template <typename T> Texture* StaticInteractable<T>::m_Shadow{ nullptr };
template <typename T> Vector2f StaticInteractable<T>::m_PickUpOffset{};
template <typename T> SoundEffect* StaticInteractable<T>::m_DestroySound{ nullptr };

template <typename T>
void StaticInteractable<T>::SetInteractableTexture(Texture* texture)
{
	m_InteractableTexture = texture;
}

template <typename T>
void StaticInteractable<T>::SetDestroyTexture(Texture* texture)
{
	m_Destroy = texture;
}

template <typename T>
void StaticInteractable<T>::SetShadowTexture(Texture* texture)
{
	m_Shadow = texture;
}

template <typename T>
Rectf StaticInteractable<T>::GetInteractableTextureRect()
{
	return Rectf{ 0.0f, 0.0f, m_InteractableTexture->GetWidth(), m_InteractableTexture->GetHeight() };
}

template <typename T>
void StaticInteractable<T>::SetPickUpOffset(const Vector2f& offset)
{
	m_PickUpOffset = offset;
}

template <typename T>
Vector2f StaticInteractable<T>::GetPickUpOffset()
{
	return m_PickUpOffset;
}

template <typename T>
void StaticInteractable<T>::SetDestroySound(SoundEffect* soundEffect)
{
	m_DestroySound = soundEffect;
}
