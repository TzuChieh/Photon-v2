#pragma once

#include "Math/math_fwd.h"

namespace ph
{

template<typename T>
class TPoint3
{
public:
	T x;
	T y;
	T z;

public:
	inline TPoint3();
	inline TPoint3(T x, T y, T z);
	inline explicit TPoint3(T value);
	inline TPoint3(const TPoint3& other);

	template<typename U>
	inline explicit TPoint3(const TPoint3<U>& other);

	template<typename U>
	inline explicit TPoint3(const TVector3<U>& vector);

	virtual inline ~TPoint3();

	inline void add(const TVector3<T>& rhs, TPoint3* out_result) const;
	inline TPoint3 add(const TVector3<T>& rhs) const;
	inline TPoint3& addLocal(const TVector3<T>& rhs);

	inline void sub(const TPoint3& rhs, TVector3<T>* out_result) const;
	inline TVector3<T> sub(const TPoint3& rhs) const;

	inline void sub(const TVector3<T>& rhs, TPoint3* out_result) const;
	inline TPoint3 sub(const TVector3<T>& rhs) const;
	inline TPoint3& subLocal(const TVector3<T>& rhs);

	inline T distance(const TPoint3& rhs) const;
	inline T distanceSquared(const TPoint3& rhs) const;
};

}// end namespace ph

#include "Math/TPoint3.ipp"