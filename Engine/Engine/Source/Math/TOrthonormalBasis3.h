#pragma once

#include "Math/math_fwd.h"
#include "Math/TVector3.h"

namespace ph::math
{

/*! @brief Represents a set of orthonormal basis vectors in 3-D space.
*/
template<typename T>
class TOrthonormalBasis3 final
{
public:
	static TOrthonormalBasis3 makeFromUnitY(const TVector3<T>& unitYAxis);

	/*! @brief Creates basis with unit x, y and z axes.
	*/
	TOrthonormalBasis3();

	/*! @brief Creates basis with custom x, y and z axes.
	All axes are expected to be normalized already.
	*/
	TOrthonormalBasis3(
		const TVector3<T>& xAxis, 
		const TVector3<T>& yAxis, 
		const TVector3<T>& zAxis);

	TVector3<T> worldToLocal(const TVector3<T>& worldVec) const;
	TVector3<T> localToWorld(const TVector3<T>& localVec) const;

	T cosPhi(const TVector3<T>& unitVec) const;
	T sinPhi(const TVector3<T>& unitVec) const;
	T tanPhi(const TVector3<T>& unitVec) const;
	T cos2Phi(const TVector3<T>& unitVec) const;
	T sin2Phi(const TVector3<T>& unitVec) const;
	T tan2Phi(const TVector3<T>& unitVec) const;
	T cosTheta(const TVector3<T>& unitVec) const;
	T sinTheta(const TVector3<T>& unitVec) const;
	T tanTheta(const TVector3<T>& unitVec) const;
	T cos2Theta(const TVector3<T>& unitVec) const;
	T sin2Theta(const TVector3<T>& unitVec) const;
	T tan2Theta(const TVector3<T>& unitVec) const;
	T absCosTheta(const TVector3<T>& unitVec) const;
	T absSinTheta(const TVector3<T>& unitVec) const;

	TOrthonormalBasis3& renormalize();
	TOrthonormalBasis3& renormalizeXAxis();
	TOrthonormalBasis3& renormalizeYAxis();
	TOrthonormalBasis3& renormalizeZAxis();

	TOrthonormalBasis3& setXAxis(const TVector3<T>& axis);
	TOrthonormalBasis3& setYAxis(const TVector3<T>& axis);
	TOrthonormalBasis3& setZAxis(const TVector3<T>& axis);

	TOrthonormalBasis3& set(
		const TVector3<T>& xAxis, 
		const TVector3<T>& yAxis, 
		const TVector3<T>& zAxis);

	TVector3<T> getXAxis() const;
	TVector3<T> getYAxis() const;
	TVector3<T> getZAxis() const;

private:
	TVector3<T> m_xAxis;
	TVector3<T> m_yAxis;
	TVector3<T> m_zAxis;
};

}// end namespace ph::math

#include "Math/TOrthonormalBasis3.ipp"
