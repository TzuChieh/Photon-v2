#pragma once

#include "Engine/Math/General/TVectorNBase.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Math/constant.h"
#include "Engine/Math/hash.h"

#include <Common/Utility/string_utils.h>

#include <functional>

namespace ph::math
{

/*! @brief Represents a 2-D vector.
*/
template<typename T>
class TVector2 final : public TVectorNBase<TVector2<T>, T, 2>
{
private:
	using Base = TVectorNBase<TVector2<T>, T, 2>;

protected:
	using Base::m;

public:
	inline TVector2() = default;
	inline TVector2(const TVector2& other) = default;
	inline TVector2(TVector2&& other) = default;
	inline TVector2& operator = (const TVector2& rhs) = default;
	inline TVector2& operator = (TVector2&& rhs) = default;
	inline ~TVector2() = default;

	using Base::Base;

	/*! @brief Creates a 2-D vector.
	*/
	TVector2(T vx, T vy);

	/*! @brief Creates a 2-D vector from another 2-D vector of different type.
	*/
	template<typename U>
	explicit TVector2(const TVector2<U>& other);

	template<typename U>
	TVector2<U> losslessCast() const;

	/*! @name Component Access (Geometric)
	*/
	///@{
	T& x();
	T& y();
	const T& x() const;
	const T& y() const;
	///@}

	/*! @name Component Access (Texture)
	*/
	///@{
	T& u();
	T& v();
	const T& u() const;
	const T& v() const;
	///@}
};

}// end namespace ph::math

template<typename T>
PH_DEFINE_INLINE_TO_STRING_FORMATTER_TEMPLATE(ph::math::TVector2<T>);

namespace std
{

template<typename T>
struct hash<ph::math::TVector2<T>>
{
	std::size_t operator () (const ph::math::TVector2<T>& vec2) const
	{
		std::size_t hash = std::hash<T>{}(vec2.x());
		hash = ph::math::combine_hashes(hash, std::hash<T>{}(vec2.y()));
		return hash;
	}
};

}// end namespace std

#include "Engine/Math/TVector2.ipp"
