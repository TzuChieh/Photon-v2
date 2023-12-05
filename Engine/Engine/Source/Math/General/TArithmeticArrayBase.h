#pragma once

#include "Math/math_fwd.h"
#include "Utility/utility.h"

#include <Common/primitive_type.h>

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

	template<typename U>
	Derived exp(U exponent) const;

	template<typename U>
	Derived& expLocal(U exponent);

	Derived exp(const Derived& exponent) const;
	Derived& expLocal(const Derived& exponent);

	Derived sqrt() const;
	Derived& sqrtLocal();

	// TODO: supply clamp methods for NaN-safe and NaN-propagative versions

	// Clamp current array's components to specific range. If a component is NaN, its value is clamped 
	// to lower bound. Either lower bound or upper bound shall not be NaN, or the method's behavior is 
	// undefined.
	Derived clamp(T lowerBound, T upperBound) const;
	Derived& clampLocal(T lowerBound, T upperBound);

	Derived abs() const;
	Derived& absLocal();

	Derived rcp() const;
	Derived& rcpLocal();

	Derived complement() const;
	Derived& complementLocal();

	T sum() const;
	T avg() const;
	T product() const;

	T min() const;
	Derived min(const Derived& other) const;
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
	bool operator == (const Derived& other) const;
	bool operator != (const Derived& other) const;

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

	auto begin() noexcept -> typename std::array<T, N>::iterator;
	auto begin() const noexcept -> typename std::array<T, N>::const_iterator;
	auto end() noexcept -> typename std::array<T, N>::iterator;
	auto end() const noexcept -> typename std::array<T, N>::const_iterator;

	std::string toString() const;
	std::vector<T> toVector() const;
	std::array<T, N> toArray() const;

protected:
	std::array<T, N> m;
};

}// end namespace ph::math

#include "Math/General/TArithmeticArrayBase.ipp"
