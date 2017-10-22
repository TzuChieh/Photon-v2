#pragma once

#include "Math/TOrthonormalBasis3.h"

namespace ph
{

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3() :
	xAxis(1, 0, 0),
	yAxis(0, 1, 0),
	zAxis(0, 0, 1)
{

}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3(const TOrthonormalBasis3& other) :
	TOrthonormalBasis3(other.xAxis, other.yAxis, other.zAxis)
{

}

template<typename T>
inline TOrthonormalBasis3<T>::TOrthonormalBasis3(
	const TVector3<T>& xAxis,
	const TVector3<T>& yAxis, 
	const TVector3<T>& zAxis) : 
	xAxis(xAxis),
	yAxis(yAxis),
	zAxis(zAxis)
{

}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::renormalize()
{
	xAxis.normalizeLocal();
	yAxis.normalizeLocal();
	zAxis.normalizeLocal();

	return *this;
}

template<typename T>
inline void TOrthonormalBasis3<T>::set(
	const TVector3<T>& xAxis,
	const TVector3<T>& yAxis,
	const TVector3<T>& zAxis)
{
	this->xAxis = xAxis;
	this->yAxis = yAxis;
	this->zAxis = zAxis;
}

template<typename T>
inline TOrthonormalBasis3<T>& TOrthonormalBasis3<T>::operator = (const TOrthonormalBasis3& rhs)
{
	set(rhs.xAxis, rhs.yAxis, rhs.zAxis);

	return *this;
}

}// end namespace ph