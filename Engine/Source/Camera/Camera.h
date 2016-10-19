#pragma once

#include "Common/primitive_type.h"
#include "Math/Vector3f.h"

namespace ph
{

class Ray;

class Camera
{
public:
	Camera();
	virtual ~Camera() = 0;

	virtual void genSampleRay(Ray* const out_ray, const uint32 xRes, const uint32 yRes, float32 x, float32 y) const = 0;

	inline const Vector3f& getPosition() const
	{
		return m_position;
	}

	inline const Vector3f& getDirection() const
	{
		return m_direction;
	}

	inline void getPosition(Vector3f* out_position) const
	{
		m_position.set(out_position);
	}

	inline void getDirection(Vector3f* out_direction) const
	{
		m_direction.set(out_direction);
	}

	inline void setPosition(const Vector3f& position)
	{
		m_position.set(position);
	}

	inline void setDirection(const Vector3f& direction)
	{
		m_direction.set(direction);
	}

private:
	Vector3f m_position;
	Vector3f m_direction;
};

}// end namespace ph