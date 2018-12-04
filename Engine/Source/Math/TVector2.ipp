#pragma once

#include "Math/TVector2.h"

#include <cmath>
#include <algorithm>

namespace ph
{

template<typename T>
inline TVector2<T>::TVector2() : 
	x(0), y(0)
{

}

template<typename T>
inline TVector2<T>::TVector2(const T x, const T y) : 
	x(x), y(y)
{

}

template<typename T>
inline TVector2<T>::TVector2(const T value) : 
	x(value), y(value)
{

}

template<typename T>
inline TVector2<T>::TVector2(const TVector2& other) : 
	x(other.x), y(other.y)
{

}

template<typename T>
template<typename U>
inline TVector2<T>::TVector2(const TVector2<U>& other) : 
	x(static_cast<T>(other.x)), y(static_cast<T>(other.y))
{

}

template<typename T>
inline TVector2<T>::~TVector2() = default;

template<typename T>
inline void TVector2<T>::add(const TVector2& rhs, TVector2* const out_result) const
{
	out_result->x = x + rhs.x;
	out_result->y = y + rhs.y;
}

template<typename T>
inline TVector2<T> TVector2<T>::add(const TVector2& rhs) const
{
	return TVector2(x + rhs.x, y + rhs.y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::addLocal(const TVector2& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::add(const T rhs) const
{
	return TVector2(x + rhs, y + rhs);
}

template<typename T>
inline TVector2<T> TVector2<T>::add(const T rhsX, const T rhsY) const
{
	return TVector2(x + rhsX, y + rhsY);
}

template<typename T>
inline void TVector2<T>::sub(const TVector2& rhs, TVector2* const out_result) const
{
	out_result->x = x - rhs.x;
	out_result->y = y - rhs.y;
}

template<typename T>
inline TVector2<T> TVector2<T>::sub(const TVector2& rhs) const
{
	return TVector2(x - rhs.x, y - rhs.y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::subLocal(const TVector2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::sub(const T rhs) const
{
	return TVector2(x - rhs, y - rhs);
}

template<typename T>
inline TVector2<T>& TVector2<T>::subLocal(T rhs)
{
	x -= rhs;
	y -= rhs;

	return *this;
}

template<typename T>
inline void TVector2<T>::mul(const TVector2& rhs, TVector2* const out_result) const
{
	out_result->x = x * rhs.x;
	out_result->y = y * rhs.y;
}

template<typename T>
inline TVector2<T> TVector2<T>::mul(const TVector2& rhs) const
{
	return TVector2(x * rhs.x, y * rhs.y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::mulLocal(const TVector2& rhs)
{
	x *= rhs.x;
	y *= rhs.y;

	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::mul(const T rhs) const
{
	return TVector2(x * rhs, y * rhs);
}

template<typename T>
inline TVector2<T>& TVector2<T>::mulLocal(T rhs)
{
	x *= rhs;
	y *= rhs;

	return *this;
}

template<typename T>
inline void TVector2<T>::div(const TVector2& rhs, TVector2* const out_result) const
{
	out_result->x = x / rhs.x;
	out_result->y = y / rhs.y;
}

template<typename T>
inline TVector2<T> TVector2<T>::div(const TVector2& rhs) const
{
	return TVector2(x / rhs.x, y / rhs.y);
}

template<typename T>
inline TVector2<T>& TVector2<T>::divLocal(const TVector2& rhs)
{
	x /= rhs.x;
	y /= rhs.y;

	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::div(const T rhs) const
{
	return TVector2(x / rhs, y / rhs);
}

template<typename T>
inline TVector2<T>& TVector2<T>::divLocal(T rhs)
{
	x /= rhs;
	y /= rhs;

	return *this;
}

template<typename T>
inline TVector2<T> TVector2<T>::min(const TVector2& other) const
{
	return TVector2(std::min(x, other.x), 
	                std::min(y, other.y));
}

template<typename T>
inline TVector2<T> TVector2<T>::max(const TVector2& other) const
{
	return TVector2(std::max(x, other.x), 
	                std::max(y, other.y));
}

template<typename T>
inline bool TVector2<T>::equals(const TVector2& other) const
{
	return x == other.x && y == other.y;
}

template<typename T>
inline bool TVector2<T>::equals(const TVector2& other, const T margin) const
{
	return std::abs(x - other.x) < margin &&
	       std::abs(y - other.y) < margin;
}

template<typename T>
inline TVector2<T> TVector2<T>::ceil() const
{
	return TVector2(static_cast<T>(std::ceil(x)), static_cast<T>(std::ceil(y)));
}

template<typename T>
inline TVector2<T> TVector2<T>::floor() const
{
	return TVector2(static_cast<T>(std::floor(x)), static_cast<T>(std::floor(y)));
}

template<typename T>
inline T TVector2<T>::product() const
{
	return x * y;
}

template<typename T>
inline std::string TVector2<T>::toString() const
{
	return '(' + std::to_string(x) + ", " + std::to_string(y) + ')';
}

template<typename T>
inline TVector2<T>& TVector2<T>::operator = (const TVector2& rhs)
{
	x = rhs.x;
	y = rhs.y;

	return *this;
}

template<typename T>
inline T TVector2<T>::operator [] (const int axisId) const
{
	switch(axisId)
	{
	case math::X_AXIS: return x;
	case math::Y_AXIS: return y;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return x;
}

template<typename T>
inline T& TVector2<T>::operator [] (const int axisId)
{
	switch(axisId)
	{
	case math::X_AXIS: return x;
	case math::Y_AXIS: return y;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return x;
}

}// end namespace ph