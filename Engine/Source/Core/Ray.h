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
class Ray : public math::TLineSegment<real>
{
public:
	class Differential;

public:
	/*! @brief A ray which state is unspecified.
	*/
	Ray();

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

	/*! @brief Points this ray in opposite direction.

	This method essentially mirrored the ray with respect to its origin, with
	other attributes remain the same.
	*/
	Ray& reverse();

	/*! @brief Set the associated time of this ray.
	*/
	void setTime(const Time& time);

	/*! @brief Get the associated time of this ray.
	*/
	const Time& getTime() const;

private:
	Time m_time;
};

// In-header Implementations:

inline Ray& Ray::reverse()
{
	flip();

	return *this;
}

inline void Ray::setTime(const Time& time)
{
	m_time = time;
}

inline const Time& Ray::getTime() const
{
	return m_time;
}

}// end namespace ph
