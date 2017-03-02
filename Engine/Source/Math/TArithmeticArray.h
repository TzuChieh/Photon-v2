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
	explicit inline TArithmeticArray(const T& value);
	explicit inline TArithmeticArray(const std::array<T, N>& values);
	inline TArithmeticArray(const TArithmeticArray& other);
	virtual inline ~TArithmeticArray();

	inline TArithmeticArray add(const TArithmeticArray& rhs) const;
	inline TArithmeticArray sub(const TArithmeticArray& rhs) const;
	inline TArithmeticArray mul(const TArithmeticArray& rhs) const;
	inline TArithmeticArray mul(const T& rhs) const;
	inline TArithmeticArray div(const TArithmeticArray& rhs) const;

	inline TArithmeticArray& addLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& subLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& mulLocal(const TArithmeticArray& rhs);
	inline TArithmeticArray& mulLocal(const T& rhs);
	inline TArithmeticArray& divLocal(const TArithmeticArray& rhs);

	inline T dot(const TArithmeticArray& rhs) const;
	inline T sum() const;

	inline T& operator [] (const std::size_t index);
	inline const T& operator [] (const std::size_t index) const;

protected:
	std::array<T, N> m;
};

}// end namespace ph