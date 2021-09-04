#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

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
	static_assert(std::is_base_of_v<TArithmeticArrayBase, Derived>,
		"Type \"Derived\" must be deriving from TArithmeticArrayBase.");

public:
	inline static constexpr auto NUM_ELEMENTS = N;

	constexpr std::size_t size() const noexcept;

// Hide members as this class is not intended to be used polymorphically
protected:
	inline TArithmeticArrayBase() = default;
	inline TArithmeticArrayBase(const TArithmeticArrayBase& other) = default;
	inline TArithmeticArrayBase(TArithmeticArrayBase&& other) = default;
	inline TArithmeticArrayBase& operator = (const TArithmeticArrayBase& rhs) = default;
	inline TArithmeticArrayBase& operator = (TArithmeticArrayBase&& rhs) = default;
	inline ~TArithmeticArrayBase() = default;

	explicit TArithmeticArrayBase(T value);
	explicit TArithmeticArrayBase(const std::array<T, N>& values);

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

	// TODO: should support floating point pow

	template<typename Integer>
	Derived pow(Integer exponent) const;

	template<typename Integer>
	Derived& powLocal(Integer exponent);

	Derived sqrt() const;
	Derived& sqrtLocal();

	// Inputs must not contain any NaN. 
	// (NaNs are clamped to lower bound)
	Derived clamp(T lowerBound, T upperBound);
	Derived& clampLocal(T lowerBound, T upperBound);

	T dot(const Derived& rhs) const;
	T dot(T rhs) const;
	T absDot(const Derived& rhs) const;
	T absDot(T rhs) const;

	T sum() const;
	T avg() const;
	T max() const;

	Derived abs() const;
	Derived& absLocal();
	Derived rcp() const;
	Derived& rcpLocal();
	Derived complement() const;
	Derived& complementLocal();

	template<typename U>
	Derived lerp(const Derived& rhs, U factor) const;

	bool isZero() const;
	bool isNonNegative() const;
	bool isFinite() const;

	Derived& set(T value);
	Derived& set(const std::array<T, N>& values);

	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

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

	Derived& operator += (const Derived& rhs) const;
	Derived& operator += (T rhs) const;
	Derived& operator -= (const Derived& rhs) const;
	Derived& operator -= (T rhs) const;
	Derived& operator *= (const Derived& rhs) const;
	Derived& operator *= (T rhs) const;
	Derived& operator /= (const Derived& rhs) const;
	Derived& operator /= (T rhs) const;

	auto begin() noexcept -> typename std::array<T, N>::iterator;
	auto begin() const noexcept -> typename std::array<T, N>::const_iterator;
	auto end() noexcept -> typename std::array<T, N>::iterator;
	auto end() const noexcept -> typename std::array<T, N>::const_iterator;

	std::string toString() const;
	std::vector<T> toVector() const;

protected:
	std::array<T, N> m;
};

}// end namespace ph::math

#include "Math/General/TArithmeticArrayBase.ipp"
