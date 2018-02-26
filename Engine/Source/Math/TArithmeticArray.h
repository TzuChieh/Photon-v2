#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"

#include <cstddef>
#include <array>

namespace ph
{

template<typename T, std::size_t N>
class TArithmeticArray
{
public:
	static inline std::size_t numElements();

public:
	inline TArithmeticArray();
	explicit inline TArithmeticArray(T value);
	explicit inline TArithmeticArray(const std::array<T, N>& values);
	inline TArithmeticArray(const TArithmeticArray& other);

	template<typename U>
	explicit inline TArithmeticArray(const TArithmeticArray<U, N>& other);

	virtual inline ~TArithmeticArray() = default;

	inline TArithmeticArray add(const TArithmeticArray& rhs) const;
	inline TArithmeticArray add(T rhs) const;
	inline TArithmeticArray sub(const TArithmeticArray& rhs) const;
	inline TArithmeticArray sub(T rhs) const;
	inline TArithmeticArray mul(const TArithmeticArray& rhs) const;
	inline TArithmeticArray mul(T rhs) const;
	inline TArithmeticArray div(const TArithmeticArray& rhs) const;
	inline TArithmeticArray div(T rhs) const;

	inline TArithmeticArray& addLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& addLocal(T rhs);
	inline TArithmeticArray& subLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& subLocal(const T rhs);
	inline TArithmeticArray& mulLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& mulLocal(T rhs);
	inline TArithmeticArray& divLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& divLocal(T rhs);

	inline TArithmeticArray pow(integer exponent) const;
	inline TArithmeticArray& sqrtLocal();

	// Inputs must not contain any NaN. 
	// (NaNs are clamped to lower bound)
	inline TArithmeticArray& clampLocal(T lowerBound, T upperBound);

	inline T dot(const TArithmeticArray& rhs) const;
	inline T sum() const;
	inline T avg() const;
	inline T max() const;
	inline TArithmeticArray complement() const;
	inline TArithmeticArray& complementLocal();

	template<typename U>
	inline TArithmeticArray<U, N> lerp(const TArithmeticArray& rhs, U factor) const;

	inline bool isZero() const;
	inline bool isNonNegative() const;
	inline std::size_t size() const;

	inline TArithmeticArray& set(const TArithmeticArray& other);
	inline TArithmeticArray& set(T value);

	inline T& operator [] (std::size_t index);
	inline const T& operator [] (std::size_t index) const;

	inline TArithmeticArray operator * (T rhs) const;
	inline TArithmeticArray operator + (T rhs) const;

	inline TArithmeticArray& operator = (const TArithmeticArray& rhs);

	inline typename std::array<T, N>::iterator       begin() noexcept;
	inline typename std::array<T, N>::const_iterator begin() const noexcept;
	inline typename std::array<T, N>::iterator       end()   noexcept;
	inline typename std::array<T, N>::const_iterator end()   const noexcept;

protected:
	std::array<T, N> m;
};

}// end namespace ph

#include "Math/TArithmeticArray.ipp"