#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/Time.h"

#include <limits>

namespace ph
{

/*! @brief Representing a ray in space.

Points $\vec{P}$ on a ray can be modeled using the equation

$$\vec{P}=\vec{O}+t\vec{D}$$

where $\vec{O}$ is ray origin and $\vec{D}$ is ray direction, and $t$ is a
parameter in $[t_{min}, t_{max}]$. This class also records the time the ray was set
on.
*/
class Ray final
{
public:
	class Differential;

public:
	Ray();
	Ray(const Vector3R& origin, const Vector3R& direction);
	Ray(const Vector3R& origin, const Vector3R& direction, real minT, real maxT);
	Ray(const Vector3R& origin, const Vector3R& direction, real minT, real maxT, const Time& time);

	/*! @brief Points this ray in opposite direction.

	This method essentially mirrored the ray with respect to its origin, with
	other attributes remain the same.
	*/
	Ray& reverse();

	void      setMinT(const real t);
	void      setMaxT(const real t);
	void      setTime(const Time& time);
	void      setDirection(const Vector3R& dir);
	void      setOrigin(const Vector3R& pos);
    Vector3R& getOrigin();
	Vector3R& getDirection();

	const Vector3R& getOrigin() const;
	const Vector3R& getDirection() const;
	const Time&     getTime() const;
	real            getMinT() const;
	real            getMaxT() const;

private:
	Vector3R m_origin;
	Vector3R m_direction;
	real     m_minT;
	real     m_maxT;
	Time     m_time;
};

// In-header Implementations:

inline Ray& Ray::reverse()
{
	m_direction.mulLocal(-1);

	return *this;
}

inline void Ray::setMinT(const real t)
{
	m_minT = t;
}

inline void Ray::setMaxT(const real t)
{
	m_maxT = t;
}

inline void Ray::setTime(const Time& time)
{
	m_time = time;
}

inline void Ray::setDirection(const Vector3R& dir)
{
	m_direction = dir;
}

inline void Ray::setOrigin(const Vector3R& pos)
{
	m_origin = pos;
}

inline Vector3R& Ray::getOrigin()
{
	return m_origin;
}

inline Vector3R& Ray::getDirection()
{
	return m_direction;
}

inline const Vector3R& Ray::getOrigin() const
{
	return m_origin;
}

inline const Vector3R& Ray::getDirection() const
{
	return m_direction;
}

inline const Time& Ray::getTime() const
{
	return m_time;
}

inline real Ray::getMinT() const
{
	return m_minT;
}

inline real Ray::getMaxT() const
{
	return m_maxT;
}

}// end namespace ph