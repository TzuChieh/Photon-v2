#pragma once

#include "Math/TVector3.h"

#include <utility>
#include <array>

namespace ph::math
{

/*! @brief Basic 3-D triangle functionalities.

The class is not intended to be used polymorphically.
*/
template<typename T>
class TBasicTriangle
{
public:
	TBasicTriangle() = default;

	TBasicTriangle(
		const TVector3<T>& vA,
		const TVector3<T>& vB,
		const TVector3<T>& vC);

	T getArea() const;
	TVector3<T> getFaceNormal() const;

	/*! @brief Calculate face normal with a fail-safe value.

	The vertices may form a degenerate triangle (zero cross product and thus
	producing NaNs after being normalized, or vertices with Inf/NaN), in such 
	case the fail-safe vector will be returned.
	*/
	TVector3<T> getFaceNormalSafe(const TVector3<T>& failSafe = {0, 1, 0}) const;

	std::pair<TVector3<T>, TVector3<T>> getEdgeVectors() const;
	TVector3<T> surfaceToBarycentric(const TVector3<T>& position) const;
	TVector3<T> barycentricToSurface(const TVector3<T>& barycentricCoords) const;

	/*! @brief Map the 2D sample to a position on the surface of the triangle.

	A common mapping on triangles which is based on slicing the triangular area
	into stripes. The mapped positions (after being converted from barycentric 
	coordinates) are distributed uniformly if the sample is uniform.

	Reference: Osada et al., Section 4.2, "Shape Distributions", TOG02
	*/
	TVector3<T> sampleToBarycentricOsada(const std::array<T, 2>& sample) const;

	bool isDegenerate() const;

	TVector3<T> getVa() const;
	TVector3<T> getVb() const;
	TVector3<T> getVc() const;

protected:
	~TBasicTriangle() = default;

	TVector3<T> m_vA;
	TVector3<T> m_vB;
	TVector3<T> m_vC;
};

}// end namespace ph::math

#include "Math/Geometry/TBasicTriangle.ipp"
