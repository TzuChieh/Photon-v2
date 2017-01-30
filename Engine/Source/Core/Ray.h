#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <limits>

#define RAY_T_EPSILON 0.0001f
#define RAY_T_MAX     std::numeric_limits<float32>::max()

namespace ph
{

class Ray final
{
public:
	Ray(const Vector3R& origin, const Vector3R& direction, const float32 minT, const float32 maxT);
	Ray(const Vector3R& origin, const Vector3R& direction);
	Ray();

	inline const Vector3R& getOrigin() const
	{
		return m_origin;
	}

	inline const Vector3R& getDirection() const
	{
		return m_direction;
	}

	inline Vector3R& getOrigin()
	{
		return m_origin;
	}

	inline Vector3R& getDirection()
	{
		return m_direction;
	}

	inline float32 getMinT() const
	{
		return m_minT;
	}

	inline float32 getMaxT() const
	{
		return m_maxT;
	}

	inline void setOrigin(const Vector3R& origin)
	{
		m_origin = origin;
	}

	inline void setDirection(const Vector3R& direction)
	{
		m_direction = direction;
	}

	inline void setMinT(const float32 minT)
	{
		m_minT = minT;
	}

	inline void setMaxT(const float32 maxT)
	{
		m_maxT = maxT;
	}

private:
	Vector3R m_origin;
	Vector3R m_direction;
	float32  m_minT;
	float32  m_maxT;
};

}// end namespace ph