#pragma once

#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/Geometry/TAABB3D.h"
#include "Utility/utility.h"

#include <cstddef>
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
	// TODO: doc & test
	static TVector3<T> interpolate(
		const std::array<TVector3<T>, 3>& attributes,
		const TVector3<T>&                barycentricCoords);

	static TVector3<T> interpolate(
		const TVector3<T>& attributeA,
		const TVector3<T>& attributeB,
		const TVector3<T>& attributeC,
		const TVector3<T>& barycentricCoords);

	// TODO: doc & test
	static bool calcSurfaceParamDerivatives(
		const std::array<TVector3<T>, 3>& attributes,
		const std::array<TVector2<T>, 3>& parameterizations,
		TVector3<T>*                      out_dXdU,
		TVector3<T>*                      out_dXdV);

public:
	TBasicTriangle(
		TVector3<T> vA,
		TVector3<T> vB,
		TVector3<T> vC);

	explicit TBasicTriangle(std::array<TVector3<T>, 3> vertices);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TBasicTriangle);

public:
	T getArea() const;
	TVector3<T> getFaceNormal() const;
	TVector3<T> getCentroid() const;
	TAABB3D<T> getAABB() const;

	/*! @brief Calculate aspect ratio of this triangle.
	For a triangle, its aspect ratio is defined to be the ratio of the circumradius to twice the
	inradius. The higher the aspect ratio, the narrower the shape of the triangle. A triangle with
	extreme aspect ratio will look like a needle. The smallest aspect ratio possible is 1 (an
	equilateral triangle), while the largest possible aspect ratio is infinity (degenerate).
	@note For integer types, returns 0 for degenerate triangles.
	*/
	T getAspectRatio() const;

	/*! @brief Calculate face normal with a fail-safe value.

	The vertices may form a degenerate triangle (zero cross product and thus
	producing NaNs after being normalized, or vertices with Inf/NaN), in such 
	case the fail-safe vector will be returned.
	*/
	TVector3<T> safeGetFaceNormal(const TVector3<T>& failSafe = {0, 1, 0}) const;

	std::pair<TVector3<T>, TVector3<T>> getEdgeVectors() const;
	TVector3<T> surfaceToBarycentric(const TVector3<T>& position) const;
	TVector3<T> barycentricToSurface(const TVector3<T>& barycentricCoords) const;

	/*! @brief Map the 2D sample to a position on the surface of the triangle.

	A common mapping on triangles which is based on slicing the triangular area
	into stripes. The mapped positions (after being converted from barycentric 
	coordinates) are distributed uniformly if the sample is uniform.

	Reference: Osada et al., Section 4.2, "Shape Distributions", TOG02

	@return Sampled barycentric coordinates.
	*/
	TVector3<T> sampleToBarycentricOsada(const std::array<T, 2>& sample) const;

	/*!
	An overload with PDF. Expected the input @p sample to be uniform.
	*/
	TVector3<T> sampleToBarycentricOsada(const std::array<T, 2>& sample, T* out_pdfA) const;

	T uniformSurfaceSamplePdfA() const;

	bool isDegenerate() const;

	TVector3<T> getVa() const;
	TVector3<T> getVb() const;
	TVector3<T> getVc() const;

private:
	TVector3<T> m_vA;
	TVector3<T> m_vB;
	TVector3<T> m_vC;
};

}// end namespace ph::math

#include "Math/Geometry/TBasicTriangle.ipp"
