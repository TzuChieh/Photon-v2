#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/Time.h"

namespace ph
{

class Ray final
{
public:
	Ray();
	Ray(const Vector3R& origin, const Vector3R& direction);
	Ray(const Vector3R& origin, const Vector3R& direction, real minT, real maxT);
	Ray(const Vector3R& origin, const Vector3R& direction, real minT, real maxT, const Time& time);

	inline const Vector3R& getOrigin() const
	{
		return m_origin;
	}

	inline const Vector3R& getDirection() const
	{
		return m_direction;
	}

	inline const Time& getTime() const
	{
		return m_time;
	}

	inline Vector3R& getOrigin()
	{
		return m_origin;
	}

	inline Vector3R& getDirection()
	{
		return m_direction;
	}

	inline real getMinT() const
	{
		return m_minT;
	}

	inline real getMaxT() const
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

	inline void setMinT(const real minT)
	{
		m_minT = minT;
	}

	inline void setMaxT(const real maxT)
	{
		m_maxT = maxT;
	}

	inline void setTime(const Time& time)
	{
		m_time = time;
	}

private:
	Vector3R m_origin;
	Vector3R m_direction;
	real     m_minT;
	real     m_maxT;
	Time     m_time;
};

}// end namespace ph