#pragma once

#include "Math/Geometry/TLineSegment.h"

#include <Common/assertion.h>

#include <limits>

namespace ph::math
{

template<typename T>
inline TLineSegment<T>::TLineSegment(const TVector3<T>& origin, const TVector3<T>& dir) : 
	TLineSegment(
		origin,
		dir,
		static_cast<T>(0),
		std::numeric_limits<T>::max())
{}

template<typename T>
inline TLineSegment<T>::TLineSegment(
	const TVector3<T>& origin, 
	const TVector3<T>& dir, 
	const T            minT, 
	const T            maxT)
{
	setOrigin(origin);
	setDir(dir);
	setRange(minT, maxT);
}

template<typename T>
inline TLineSegment<T>& TLineSegment<T>::flip()
{
	m_dir.mulLocal(T(-1));

	return *this;
}

template<typename T>
inline void TLineSegment<T>::setMinT(const T t)
{
	m_minT = t;
}

template<typename T>
inline void TLineSegment<T>::setMaxT(const T t)
{
	m_maxT = t;
}

template<typename T>
inline void TLineSegment<T>::setRange(const T minT, const T maxT)
{
	PH_ASSERT_LE(minT, maxT);

	setMinT(minT);
	setMaxT(maxT);
}

template<typename T>
inline void TLineSegment<T>::setRange(const std::pair<T, T>& minMaxT)
{
	setRange(minMaxT.first, minMaxT.second);
}

template<typename T>
inline void TLineSegment<T>::setOrigin(const TVector3<T>& pos)
{
	m_origin = pos;
}

template<typename T>
inline void TLineSegment<T>::setDir(const TVector3<T>& dir)
{
	m_dir = dir;
}

template<typename T>
inline const TVector3<T>& TLineSegment<T>::getOrigin() const
{
	return m_origin;
}

template<typename T>
inline const TVector3<T>& TLineSegment<T>::getDir() const
{
	return m_dir;
}

template<typename T>
inline T TLineSegment<T>::getMinT() const
{
	return m_minT;
}

template<typename T>
inline T TLineSegment<T>::getMaxT() const
{
	return m_maxT;
}

template<typename T>
inline std::pair<T, T> TLineSegment<T>::getRange() const
{
	return {m_minT, m_maxT};
}

template<typename T>
inline TVector3<T> TLineSegment<T>::getTail() const
{
	return m_dir * m_minT + m_origin;
}

template<typename T>
inline TVector3<T> TLineSegment<T>::getHead() const
{
	return m_dir * m_maxT + m_origin;
}

template<typename T>
inline TVector3<T> TLineSegment<T>::getPoint(const T t) const
{
	return m_origin.add(m_dir.mul(t));
}

}// end namespace ph::math
