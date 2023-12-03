#pragma once

#include "Math/Geometry/TLineSegment.h"
#include "Math/math.h"
#include "Math/math_fwd.h"
#include "Math/Geometry/TAABB3D.h"

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

	bool isInside(const TVector3<T>& point) const;

	T getArea() const;
	TAABB3D<T> getAABB() const;

	/*! @brief Conservatively checks whether this sphere overlaps a volume.

	By conservative, it means **true can be returned even though the sphere does not overlap 
	the volume**; but if it actually does, **true must be returned**. The test considers the 
	sphere as **hollow** and the volume is **solid**.
	*/
	bool mayOverlapVolume(const TAABB3D<T>& volume) const;

	/*! @brief Map the 2D sample to a position on the surface of the sphere.
	
	A common mapping method that is based on Archimedes' derivation that 
	the horizontal slices of a sphere have equal area. The mapped positions
	are distributed uniformly if the sample is uniform. For a unit sphere,
	this method effectively generates normalized directions.
	*/
	TVector3<T> sampleToSurfaceArchimedes(const std::array<T, 2>& sample) const;

	/*!
	An overload with PDF. Expected the input @p sample to be uniform.
	*/
	TVector3<T> sampleToSurfaceArchimedes(const std::array<T, 2>& sample, T* out_pdfA) const;

	T uniformSurfaceSamplePdfA() const;

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
	/*! Naive and striaghtforward sphere-line intersection. */
	bool isIntersectingNaive(
		const TLineSegment<T>& segment,
		real* out_hitT) const;

	/*! Hearn and Baker's formulation of sphere-line intersection. This is more numerically robust. */
	bool isIntersectingHearnBaker(
		const TLineSegment<T>& segment,
		real* out_hitT) const;

	T m_radius;
};

}// end namespace ph::math

#include "Math/Geometry/TSphere.ipp"
