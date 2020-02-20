#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/Geometry/TLineSegment.h"
#include "Core/Quantity/Time.h"

#include <limits>

namespace ph
{

/*! @brief Represents a ray in space.

A ray is essentially a line segment in space with additional attributes 
(for example, time). Note the direction vector of the ray does not need to 
be normalized.
*/
class Ray final
{
public:
	class Differential;

public:
	/*! @brief A ray which state is unspecified.
	*/
	Ray() = default;

	/*! @brief A longest possible ray.

	@param direction Does not need to be normalized.
	*/
	Ray(const math::Vector3R& origin, const math::Vector3R& direction);

	/*! @brief A ray segment.

	@param minT Parametric distance where the ray begins.
	@param maxT Parametric distance where the ray ends.
	*/
	Ray(const math::Vector3R& origin, const math::Vector3R& direction, real minT, real maxT);

	/*! @copydoc Ray::Ray(const Vector3R&, const Vector3R&, real, real)

	@param time The associated time of this ray.
	*/
	Ray(const math::Vector3R& origin, const math::Vector3R& direction, real minT, real maxT, const Time& time);

	// TODO: doc
	Ray(const math::TLineSegment<real>& segment, const Time& time);

	/*! @brief Points this ray in opposite direction.

	This method essentially mirrored the ray with respect to its origin, with
	other attributes remain the same.
	*/
	Ray& reverse();

	/*! @brief Set the parametric distance where the ray starts.
	*/
	void setMinT(real t);

	/*! @brief Set the parametric distance where the ray ends.
	*/
	void setMaxT(real t);

	/*! @brief Set the parametric range where the ray extends.

	The range is [minT, maxT). This is equivalent to calling \ref setMinT(real) 
	and \ref setMaxT(real) together.
	*/
	void setRange(real minT, real maxT);

	/*! @brief Set the origin of the ray.
	*/
	void setOrigin(const math::Vector3R& pos);

	/*! @brief Set the direction vector of the ray.

	Note that the vector does not need to be normalized.
	*/
	void setDirection(const math::Vector3R& dir);

	void setSegment(const math::TLineSegment<real>& segment);

	/*! @name Getters

	Basic getters for line attributes. See corresponding setters (if present)
	for more info.
	*/
	///@{
	const math::Vector3R& getOrigin() const;
	const math::Vector3R& getDirection() const;
	real getMinT() const;
	real getMaxT() const;
	const math::TLineSegment<real>& getSegment() const;

	/*! @brief Get the coordinates on minimum parametric distance.
	*/
	math::Vector3R getTail() const;

	/*! @brief Get the coordinates on maximum parametric distance.
	*/
	math::Vector3R getHead() const;
	///@}

	/*! @brief Set the associated time of this ray.
	*/
	void setTime(const Time& time);

	/*! @brief Get the associated time of this ray.
	*/
	const Time& getTime() const;

private:
	math::TLineSegment<real> m_segment;
	Time                     m_time;
};

// In-header Implementations:

inline Ray::Ray(const math::TLineSegment<real>& segment, const Time& time) :
	m_segment(segment), m_time(time)
{}

inline Ray::Ray(
	const math::Vector3R& origin, const math::Vector3R& direction,
	const real minT, const real maxT,
	const Time& time) :

	Ray(
		math::TLineSegment<real>(
			origin, direction,
			minT, maxT),
		time)
{}

inline Ray::Ray(
	const math::Vector3R& origin, const math::Vector3R& direction,
	const real minT, const real maxT) :

	Ray(origin, direction, minT, maxT, Time())
{}

inline Ray::Ray(const math::Vector3R& origin, const math::Vector3R& direction) :
	Ray(origin, direction, 0, std::numeric_limits<real>::max())
{}

inline Ray& Ray::reverse()
{
	m_segment.flip();

	return *this;
}

inline void Ray::setMinT(const real t)
{
	m_segment.setMinT(t);
}

inline void Ray::setMaxT(const real t)
{
	m_segment.setMaxT(t);
}

inline void Ray::setRange(const real minT, const real maxT)
{
	m_segment.setRange(minT, maxT);
}

inline void Ray::setOrigin(const math::Vector3R& pos)
{
	m_segment.setOrigin(pos);
}

inline void Ray::setDirection(const math::Vector3R& dir)
{
	m_segment.setDirection(dir);
}

inline void Ray::setSegment(const math::TLineSegment<real>& segment)
{
	m_segment = segment;
}

inline void Ray::setTime(const Time& time)
{
	m_time = time;
}

inline const Time& Ray::getTime() const
{
	return m_time;
}

inline const math::Vector3R& Ray::getOrigin() const
{
	return m_segment.getOrigin();
}

inline const math::Vector3R& Ray::getDirection() const
{
	return m_segment.getDirection();
}

inline real Ray::getMinT() const
{
	return m_segment.getMinT();
}

inline real Ray::getMaxT() const
{
	return m_segment.getMaxT();
}

inline const math::TLineSegment<real>& Ray::getSegment() const
{
	return m_segment;
}

inline math::Vector3R Ray::getTail() const
{
	return m_segment.getTail();
}

inline math::Vector3R Ray::getHead() const
{
	return m_segment.getHead();
}

}// end namespace ph
