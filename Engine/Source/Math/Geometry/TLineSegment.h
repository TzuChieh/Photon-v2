#pragma once

#include "Math/TVector3.h"

#include <utility>

namespace ph::math
{

/*! @brief Represents a line segment in space.

Points @f$ \overrightarrow{P} @f$ on a line are modeled using the equation

@f[
	\overrightarrow{P} = \overrightarrow{O} + t\overrightarrow{D}
@f]

where @f$ \overrightarrow{O} @f$ is the line origin and 
@f$ \overrightarrow{D} @f$ is the line direction, and @f$ t @f$ is a parameter
in @f$ [t_{min}, t_{max}) @f$. Note the direction vector of the line does not
need to be normalized.
*/
template<typename T>
class TLineSegment final
{
public:
	/*! @brief A line which state is unspecified.
	*/
	TLineSegment() = default;

	/*! @brief A line that covers the parametric range [minT, maxT).

	@param direction Line direction. Does not need to be normalized.
	*/
	TLineSegment(
		const TVector3<T>& origin, 
		const TVector3<T>& direction, 
		T                  minT, 
		T                  maxT);

	/*! @brief Point the line in opposite direction.

	This method essentially mirrored the line with respect to its origin, with
	other attributes remain the same.
	*/
	TLineSegment& flip();

	/*! @brief Set the parametric distance where the segment starts.
	*/
	void setMinT(T t);

	/*! @brief Set the parametric distance where the segment ends.
	*/
	void setMaxT(T t);

	/*! @brief Set the parametric range where the segment extends.

	The range is [minT, maxT). This is equivalent to calling \ref setMinT(T) 
	and \ref setMaxT(T) together.
	*/
	void setRange(T minT, T maxT);

	/*! @brief Same as \ref setRange(T, T).
	*/
	void setRange(const std::pair<T, T>& minMaxT);

	/*! @brief Set the origin of the line.
	*/
	void setOrigin(const TVector3<T>& pos);

	/*! @brief Set the direction vector of the line.

	Note that the vector does not need to be normalized.
	*/
	void setDirection(const TVector3<T>& dir);

	/*! @name Getters

	Basic getters for line attributes. See corresponding setters (if present)
	for more info.
	*/
	///@{
	const TVector3<T>& getOrigin() const;
	const TVector3<T>& getDirection() const;
	T getMinT() const;
	T getMaxT() const;
	std::pair<T, T> getRange() const;

	/*! @brief Get the coordinates on minimum parametric distance.
	*/
	TVector3<T> getTail() const;

	/*! @brief Get the coordinates on maximum parametric distance.
	*/
	TVector3<T> getHead() const;
	///@}

private:
	TVector3<T> m_origin;
	TVector3<T> m_direction;
	T           m_minT;
	T           m_maxT;
};

}// end namespace ph::math

#include "Math/Geometry/TLineSegment.ipp"
