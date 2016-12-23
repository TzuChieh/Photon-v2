#pragma once

#include "Math/Vector3f.h"

#include <limits>

namespace ph
{

class Ray final
{
public:
	Ray(const Vector3f& origin, const Vector3f& direction, const float32 maxT = std::numeric_limits<float32>::max());
	Ray();

	inline const Vector3f& getOrigin() const
	{
		return m_origin;
	}

	inline const Vector3f& getDirection() const
	{
		return m_direction;
	}

	inline Vector3f& getOrigin()
	{
		return m_origin;
	}

	inline Vector3f& getDirection()
	{
		return m_direction;
	}

	inline float32 getMaxT() const
	{
		return m_maxT;
	}

	inline void setOrigin(const Vector3f& origin)
	{
		m_origin = origin;
	}

	inline void setDirection(const Vector3f& direction)
	{
		m_direction = direction;
	}

	inline void setMaxT(const float32 maxT)
	{
		m_maxT = maxT;
	}

private:
	Vector3f m_origin;
	Vector3f m_direction;
	float32  m_maxT;
};

}// end namespace ph