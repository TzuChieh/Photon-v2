#pragma once

#include "Math/Geometry/TLineSegment.h"
#include "Math/math.h"

#include <array>
#include <utility>

namespace ph::math
{

/*! @brief A sphere in 3-D space.

The center of the sphere is at the origin.
*/
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
	TVector3<T> sampleToSurfaceArchimedes(const std::array<T, 2>& sample) const;
	TVector3<T> sampleToSurfaceArchimedes(const std::array<T, 2>& sample, T* out_pdfA) const;

	TVector2<T> surfaceToLatLong01(const TVector3<T>& surface) const;
	TVector2<T> latLong01ToPhiTheta(const TVector2<T>& latLong01) const;
	TVector3<T> latLong01ToSurface(const TVector2<T>& latLong01) const;

	/*! @brief Map Cartesian to spherical coordinates on the surface of the sphere.
	*/
	TVector2<T> surfaceToPhiTheta(const TVector3<T>& surface) const;

	/*! @brief Map spherical to Cartesian coordinates on the surface of the sphere.
	*/
	TVector3<T> phiThetaToSurface(const TVector2<T>& phiTheta) const;

	/*! @brief Calculate dPdU and dPdV with finite difference.

	This is a partial derivatives calculator for arbitrary UV maps; however,
	as it is only a finite difference approximation, artifacts can occur around
	the discontinuities of the underlying UV mapping.

	@param surface Coordinates of the point of interest on the surface.
	@param surfaceToUv A mapper that maps surface coordinates to UV.
	@param hInRadians Half inverval in radians used in the finite difference.
	Should within (0, pi/4].
	@return A pair containing dPdU and dPdV.
	*/
	template<typename SurfaceToUv>
	std::pair<TVector3<T>, TVector3<T>> surfaceDerivativesWrtUv(
		const TVector3<T>& surface, 
		SurfaceToUv        surfaceToUv, 
		T                  hInRadians = to_radians<T>(1)) const;

private:
	T m_radius;
};

}// end namespace ph::math

#include "Math/Geometry/TSphere.ipp"
