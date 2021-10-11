#pragma once
class Map;

class Camera final
{
public:
	Camera(float width, float height, float scale);
	Rectf CameraTransform() const;
	void Transform(const Rectf& target, const Map* const map, const bool free);
	void CenterOn(const Rectf& target, const Map* const map);
private:
	const float m_Width;
	const float m_Height;
	const float m_Scale;
	Rectf m_FreeMovement;

	Point2f Track(const Rectf& target);
	bool Clamp(Point2f& bottomLeftPos, const Map* const map) const;
	void Free(const Rectf& target, const Map*);
	void MoveFreeTransform(const Rectf& target);
	void ClampFree(const Map* const map);
	Point2f CameraBottomLeft() const;
	void FreeFromCameraBottomLeft(const Point2f& bottomLeft);
};
