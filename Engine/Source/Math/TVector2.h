#pragma once

#include "Math/math_fwd.h"

#include <string>

namespace ph
{

template<typename T>
class TVector2 final
{
public:
	T x;
	T y;

	inline TVector2() = default;
	inline TVector2(T x, T y);
	inline explicit TVector2(T value);
	inline TVector2(const TVector2& other) = default;

	template<typename U>
	explicit inline TVector2(const TVector2<U>& other);

	inline void add(const TVector2& rhs, TVector2* out_result) const;
	inline TVector2 add(const TVector2& rhs) const;
	inline TVector2& addLocal(const TVector2& rhs);
	inline TVector2 add(T rhs) const;
	inline TVector2 add(T rhsX, T rhsY) const;

	inline void sub(const TVector2& rhs, TVector2* out_result) const;
	inline TVector2 sub(const TVector2& rhs) const;
	inline TVector2& subLocal(const TVector2& rhs);
	inline TVector2 sub(T rhs) const;
	inline TVector2& subLocal(T rhs);

	inline void mul(const TVector2& rhs, TVector2* out_result) const;
	inline TVector2 mul(const TVector2& rhs) const;
	inline TVector2& mulLocal(const TVector2& rhs);
	inline TVector2 mul(T rhs) const;
	inline TVector2& mulLocal(T rhs);

	inline void div(const TVector2& rhs, TVector2* out_result) const;
	inline TVector2 div(const TVector2& rhs) const;
	inline TVector2& divLocal(const TVector2& rhs);
	inline TVector2 div(T rhs) const;
	inline TVector2& divLocal(T rhs);

	inline TVector2 min(const TVector2& other) const;
	inline TVector2 max(const TVector2& other) const;

	inline bool equals(const TVector2& other) const;
	inline bool equals(const TVector2& other, T margin) const;

	inline TVector2 ceil() const;
	inline TVector2 floor() const;
	T product() const;

	inline std::string toString() const;

	inline T operator [] (int axisId) const;
	inline T& operator [] (int axisId);

	TVector2 operator + (const TVector2& rhs) const;
	TVector2 operator + (T rhs) const;
	TVector2 operator - (const TVector2& rhs) const;
	TVector2 operator - (T rhs) const;
	TVector2 operator * (const TVector2& rhs) const;
	TVector2 operator * (T rhs) const;
	TVector2 operator / (const TVector2& rhs) const;
	TVector2 operator / (T rhs) const;
};

}// end namespace ph

#include "Math/TVector2.ipp"