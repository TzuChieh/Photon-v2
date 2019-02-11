#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <cstddef>
#include <array>
#include <string>
#include <initializer_list>

namespace ph
{

template<typename T, std::size_t N>
class TArithmeticArray final
{
public:
	static std::size_t numElements();

public:
	inline TArithmeticArray() = default;
	explicit TArithmeticArray(T value);
	explicit TArithmeticArray(const std::array<T, N>& values);
	inline TArithmeticArray(const TArithmeticArray& other) = default;

	template<typename U>
	explicit TArithmeticArray(const TArithmeticArray<U, N>& other);

	TArithmeticArray add(const TArithmeticArray& rhs) const;
	TArithmeticArray add(T rhs) const;
	TArithmeticArray sub(const TArithmeticArray& rhs) const;
	TArithmeticArray sub(T rhs) const;
	TArithmeticArray mul(const TArithmeticArray& rhs) const;
	TArithmeticArray mul(T rhs) const;
	TArithmeticArray div(const TArithmeticArray& rhs) const;
	TArithmeticArray div(T rhs) const;

	TArithmeticArray& addLocal(const TArithmeticArray& rhs);
	TArithmeticArray& addLocal(T rhs);
	TArithmeticArray& subLocal(const TArithmeticArray& rhs);
	TArithmeticArray& subLocal(T rhs);
	TArithmeticArray& mulLocal(const TArithmeticArray& rhs);
	TArithmeticArray& mulLocal(T rhs);
	TArithmeticArray& divLocal(const TArithmeticArray& rhs);
	TArithmeticArray& divLocal(T rhs);

	TArithmeticArray pow(integer exponent) const;
	TArithmeticArray& sqrtLocal();

	// Inputs must not contain any NaN. 
	// (NaNs are clamped to lower bound)
	TArithmeticArray& clampLocal(T lowerBound, T upperBound);

	T dot(const TArithmeticArray& rhs) const;
	T sum() const;
	T avg() const;
	T max() const;
	TArithmeticArray abs() const;
	TArithmeticArray complement() const;
	TArithmeticArray& complementLocal();

	template<typename U>
	TArithmeticArray<U, N> lerp(const TArithmeticArray& rhs, U factor) const;

	bool isZero() const;
	bool isNonNegative() const;
	bool isFinite() const;
	constexpr std::size_t size() const noexcept;

	TArithmeticArray& set(T value);
	TArithmeticArray& set(const std::array<T, N>& values);

	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;
	bool operator == (const TArithmeticArray& other) const;

	TArithmeticArray operator * (T rhs) const;
	TArithmeticArray operator + (T rhs) const;

	auto begin() noexcept       -> typename std::array<T, N>::iterator;
	auto begin() const noexcept -> typename std::array<T, N>::const_iterator;
	auto end() noexcept         -> typename std::array<T, N>::iterator;
	auto end() const noexcept   -> typename std::array<T, N>::const_iterator;

	std::string toString() const;

protected:
	std::array<T, N> m;
};

}// end namespace ph

#include "Math/TArithmeticArray.ipp"