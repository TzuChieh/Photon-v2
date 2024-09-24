#pragma once

#include "Math/math_fwd.h"
#include "Utility/utility.h"
#include "Utility/TSpan.h"

#include <Common/primitive_type.h>
#include <Common/compiler.h>

#include <cstddef>
#include <array>
#include <type_traits>
#include <string>
#include <vector>

namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
class TArithmeticArrayBase
{
private:
	using Self = TArithmeticArrayBase;

public:
	using Elements = std::array<T, N>;

	explicit TArithmeticArrayBase(T value);
	explicit TArithmeticArrayBase(Elements values);

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TArithmeticArrayBase);

protected:
	inline static constexpr auto NUM_ELEMENTS = N;

	/*! @brief Number of elements of the array.
	*/
	constexpr std::size_t size() const noexcept;

	Derived add(const Derived& rhs) const;
	Derived add(T rhs) const;
	Derived& addLocal(const Derived& rhs);
	Derived& addLocal(T rhs);

	Derived sub(const Derived& rhs) const;
	Derived sub(T rhs) const;
	Derived& subLocal(const Derived& rhs);
	Derived& subLocal(T rhs);

	Derived mul(const Derived& rhs) const;
	Derived mul(T rhs) const;
	Derived& mulLocal(const Derived& rhs);
	Derived& mulLocal(T rhs);

	Derived div(const Derived& rhs) const;
	Derived div(T rhs) const;
	Derived& divLocal(const Derived& rhs);
	Derived& divLocal(T rhs);

	template<typename U>
	Derived pow(U exponent) const;

	template<typename U>
	Derived& powLocal(U exponent);

	Derived pow(const Derived& exponent) const;
	Derived& powLocal(const Derived& exponent);

	/*! @brief Sets the array to @f$ e^exponent @f$.
	*/
	///@{
	template<typename U>
	Derived exp(U exponent) const;

	template<typename U>
	Derived& expLocal(U exponent);
	///@}

	/*! @brief Sets the array to @f$ e^{exponent_i} @f$.
	*/
	///@{
	Derived exp(const Derived& exponent) const;
	Derived& expLocal(const Derived& exponent);
	///@}

	Derived sqrt() const;
	Derived& sqrtLocal();

	// TODO: supply clamp methods for NaN-safe and NaN-propagative versions

	/*! @brief Clamps current array's elements to specific range.
	None of `value`, `lowerBound` and `upperBound` can be NaN, or the method's behavior is undefined.
	*/
	///@{
	Derived clamp(T lowerBound, T upperBound) const;
	Derived& clampLocal(T lowerBound, T upperBound);
	Derived clamp(const Derived& lowerBound, const Derived& upperBound) const;
	Derived& clampLocal(const Derived& lowerBound, const Derived& upperBound);
	///@}

	/*! @brief Clamps current array's elements to specific range.
	If a floating-point value is non-finite (e.g., being Inf, NaN), its value is clamped to `lowerBound`.
	Neither `lowerBound` nor `upperBound` can be NaN, or the method's behavior is undefined.
	*/
	///@{
	Derived safeClamp(T lowerBound, T upperBound) const;
	Derived& safeClampLocal(T lowerBound, T upperBound);
	Derived safeClamp(const Derived& lowerBound, const Derived& upperBound) const;
	Derived& safeClampLocal(const Derived& lowerBound, const Derived& upperBound);
	///@}

	Derived abs() const;
	Derived& absLocal();

	Derived rcp() const;
	Derived& rcpLocal();

	/*! @brief Complements the array's elements.
	Effectively performing `1 - (*this)[i]` for each element.
	*/
	///@{
	Derived complement() const;
	Derived& complementLocal();
	///@}

	/*! @brief Applies a negative sign to the array's elements.
	These methods is only defined for signed element types.
	*/
	///@{
	Derived negate() const
	requires std::is_signed_v<T>;

	Derived& negateLocal()
	requires std::is_signed_v<T>;
	///@}

	T sum() const;
	T avg() const;
	T product() const;

	/*!
	@return Minimum of the elements.
	*/
	T min() const;

	/*!
	@return Element-wise minimum. Effectively performing `std::min((*this)[i], other[i])` for each element.
	*/
	Derived min(const Derived& other) const;

	/*!
	@return Index of the minimum.
	*/
	std::size_t minIndex() const;

	T max() const;
	Derived max(const Derived& other) const;
	std::size_t maxIndex() const;

	Derived ceil() const;
	Derived floor() const;

	template<typename U>
	Derived lerp(const Derived& rhs, U factor) const;

	bool isZero() const;
	bool isNonNegative() const;
	bool isFinite() const;

	Derived& set(T value);
	Derived& set(std::size_t index, T value);
	Derived& set(const std::array<T, N>& values);

	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

	bool isEqual(const Derived& other) const;
	bool isNear(const Derived& other, T margin) const;

	/*!
	@return `true` if the elements are equal to those in `other`, `false` otherwise.
	@note To support C++20's rewritten comparison operators without ambiguity, `other` is not of
	`Derived` type. If this is undesirable, use `isEqual()`.
	*/
	bool operator == (const Self& other) const;

#if !PH_COMPILER_HAS_P2468R2
	/*!
	@return `true` if the elements are not equal to those in `other`, `false` otherwise.
	@note To support C++20's rewritten comparison operators without ambiguity, `other` is not of
	`Derived` type. If this is undesirable, use `!isEqual()`.
	*/
	bool operator != (const Self& other) const;
#endif

	Derived operator + (const Derived& rhs) const;
	Derived operator + (T rhs) const;
	Derived operator - (const Derived& rhs) const;
	Derived operator - (T rhs) const;
	Derived operator * (const Derived& rhs) const;
	Derived operator * (T rhs) const;
	Derived operator / (const Derived& rhs) const;
	Derived operator / (T rhs) const;

	Derived& operator += (const Derived& rhs);
	Derived& operator += (T rhs);
	Derived& operator -= (const Derived& rhs);
	Derived& operator -= (T rhs);
	Derived& operator *= (const Derived& rhs);
	Derived& operator *= (T rhs);
	Derived& operator /= (const Derived& rhs);
	Derived& operator /= (T rhs);

	Derived operator - () const
	requires std::is_signed_v<T>;

	auto begin() noexcept -> typename std::array<T, N>::iterator;
	auto begin() const noexcept -> typename std::array<T, N>::const_iterator;
	auto end() noexcept -> typename std::array<T, N>::iterator;
	auto end() const noexcept -> typename std::array<T, N>::const_iterator;

	std::string toString() const;
	std::vector<T> toVector() const;
	std::array<T, N> toArray() const;
	TSpan<T, N> toSpan();
	TSpanView<T, N> toView() const;

protected:
	std::array<T, N> m;
};

}// end namespace ph::math

#include "Math/General/TArithmeticArrayBase.ipp"
