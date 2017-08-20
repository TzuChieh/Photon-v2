#pragma once

#include <cstddef>
#include <array>

namespace ph
{

template<typename T, std::size_t N>
class TArithmeticArray
{
public:
	inline TArithmeticArray();
	explicit inline TArithmeticArray(T value);
	explicit inline TArithmeticArray(const std::array<T, N>& values);
	inline TArithmeticArray(const TArithmeticArray& other);
	virtual inline ~TArithmeticArray();

	inline TArithmeticArray add(const TArithmeticArray& rhs) const;
	inline TArithmeticArray sub(const TArithmeticArray& rhs) const;
	inline TArithmeticArray mul(const TArithmeticArray& rhs) const;
	inline TArithmeticArray mul(T rhs) const;
	inline TArithmeticArray div(const TArithmeticArray& rhs) const;

	inline TArithmeticArray& addLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& subLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& mulLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& mulLocal(T rhs);
	inline TArithmeticArray& divLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& divLocal(T rhs);

	// Inputs must not contain any NaN. 
	// (NaNs are clamped to lower bound)
	inline TArithmeticArray& clampLocal(T lowerBound, T upperBound);

	inline T dot(const TArithmeticArray& rhs) const;
	inline T sum() const;
	inline T avg() const;
	inline TArithmeticArray complement() const;
	inline TArithmeticArray& complementLocal();

	inline bool isZero() const;
	inline bool isNonNegative() const;

	inline TArithmeticArray& set(const TArithmeticArray& other);
	inline TArithmeticArray& set(T value);

	inline T& operator [] (std::size_t index);
	inline const T& operator [] (std::size_t index) const;

	inline TArithmeticArray& operator = (const TArithmeticArray& rhs);

protected:
	std::array<T, N> m;
};

}// end namespace ph

#include "Math/TArithmeticArray.ipp"