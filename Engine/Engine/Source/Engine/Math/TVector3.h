#pragma once

#include "Engine/Math/General/TVectorNBase.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Math/constant.h"
#include "Engine/Utility/utility.h"

#include <Common/Utility/string_utils.h>

namespace ph::math
{

/*! @brief Represents a 3-D vector.
*/
template<typename T>
class TVector3 final : public TVectorNBase<TVector3<T>, T, 3>
{
private:
	using Base = TVectorNBase<TVector3<T>, T, 3>;

protected:
	using Base::m;

public:
	static TVector3 weightedSum(
		const TVector3& vA, T wA, 
		const TVector3& vB, T wB, 
		const TVector3& vC, T wC);

	static TVector3 lerp(
		const TVector3& vA, 
		const TVector3& vB, 
		T               parametricT);

public:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TVector3);

	using Base::Base;

	/*! @brief Creates a 3-D vector.
	*/
	TVector3(T vx, T vy, T vz);

	/*! @brief Creates a 3-D vector from another 3-D vector of different type.
	*/
	template<typename U>
	explicit TVector3(const TVector3<U>& other);

	TVector3 rotate(const TQuaternion<T>& rotation) const;
	void rotate(const TQuaternion<T>& rotation, TVector3* out_result) const;

	TVector3 cross(const TVector3& rhs) const;
	void cross(const TVector3& rhs, TVector3* out_result) const;

	TVector3& maddLocal(T multiplier, const TVector3& adder);
	
	/*! @brief Calculate the reflected vector.
	For example, if the surface normal is @f$ N @f$ and this vector is @f$ -N @f$, the reflected vector
	will be @f$ N @f$.
	@param normal Normal of the surface to reflect this vector.
	@return Reflected vector.
	*/
	///@{
	TVector3 reflect(const TVector3& normal) const;
	TVector3& reflectLocal(const TVector3& normal);
	///@}

	/*!
	@param out_result The sorted values, (x, y, z) = (min, mid, max).
	*/
	void sort(TVector3* out_result) const;

	/*! @name Component Access (Geometric)
	*/
	///@{
	T& x();
	T& y();
	T& z();
	const T& x() const;
	const T& y() const;
	const T& z() const;
	///@}

	/*! @name Component Access (Color)
	*/
	///@{
	T& r();
	T& g();
	T& b();
	const T& r() const;
	const T& g() const;
	const T& b() const;
	///@}
};

}// end namespace ph::math

template<typename T>
PH_DEFINE_INLINE_TO_STRING_FORMATTER_TEMPLATE(ph::math::TVector3<T>);

#include "Engine/Math/TVector3.ipp"
