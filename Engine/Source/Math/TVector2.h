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
	TVector2(T x, T y);
	explicit TVector2(T value);
	inline TVector2(const TVector2& other) = default;

	template<typename U>
	explicit TVector2(const TVector2<U>& other);

	void add(const TVector2& rhs, TVector2* out_result) const;
	TVector2 add(const TVector2& rhs) const;
	TVector2& addLocal(const TVector2& rhs);
	TVector2 add(T rhs) const;
	TVector2 add(T rhsX, T rhsY) const;

	void sub(const TVector2& rhs, TVector2* out_result) const;
	TVector2 sub(const TVector2& rhs) const;
	TVector2& subLocal(const TVector2& rhs);
	TVector2 sub(T rhs) const;
	TVector2& subLocal(T rhs);

	void mul(const TVector2& rhs, TVector2* out_result) const;
	TVector2 mul(const TVector2& rhs) const;
	TVector2& mulLocal(const TVector2& rhs);
	TVector2 mul(T rhs) const;
	TVector2& mulLocal(T rhs);

	void div(const TVector2& rhs, TVector2* out_result) const;
	TVector2 div(const TVector2& rhs) const;
	TVector2& divLocal(const TVector2& rhs);
	TVector2 div(T rhs) const;
	TVector2& divLocal(T rhs);

	TVector2 min(const TVector2& other) const;
	TVector2 max(const TVector2& other) const;
	int maxDimension() const;

	bool equals(const TVector2& other) const;
	bool equals(const TVector2& other, T margin) const;

	TVector2 ceil() const;
	TVector2 floor() const;
	T product() const;

	TVector2& set(int axis, T value);

	std::string toString() const;

	T operator [] (int axisId) const;
	T& operator [] (int axisId);

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