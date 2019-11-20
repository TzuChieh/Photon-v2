#pragma once

#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TLineSegment.h"

namespace ph::math
{

template<typename T>
class TSphere final
{
public:
	static TSphere makeUnit();

	TSphere() = default;

	explicit TSphere(T radius);

	bool isIntersecting(
		const TLineSegment<T>& segment,
		real*                  out_hitT) const;

	T getArea() const;

	/*! @brief Map the 2D sample to a position on the surface of the sphere.
	
	A common mapping method that is based on Archimedes' derivation that 
	the horizontal slices of a sphere have equal area. The mapped positions
	are distributed uniformly if the sample is uniform. For a unit sphere,
	this method effectively generates normalized directions.
	*/
	TVector3<T> sampleToSurfaceArchimedes(const TVector2<T>& sample) const;

private:
	T m_radius;
};

}// end namespace ph::math

#include "Math/Geometry/TSphere.ipp"
