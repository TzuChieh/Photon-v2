#pragma once

#include "Math/Geometry/TBasicTriangle.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph::math
{

template<typename T>
inline TBasicTriangle<T>::TBasicTriangle(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC) : 

	m_vA(vA),
	m_vB(vB),
	m_vC(vC)
{}

template<typename T>
inline T TBasicTriangle<T>::getArea() const
{
	const auto [eAB, eAC] = getEdgeVectors();

	return eAB.cross(eAC).length() * T(0.5);
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::getFaceNormal() const
{
	const auto [eAB, eAC] = getEdgeVectors();

	return eAB.cross(eAC).normalizeLocal();
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::getFaceNormalSafe(const TVector3<T>& failSafe) const
{
	return !isDegenerate() ? getFaceNormal() : failSafe;
}

template<typename T>
inline std::pair<TVector3<T>, TVector3<T>> TBasicTriangle<T>::getEdgeVectors() const
{
	return {
		m_vB.sub(m_vA),// edge vector AB
		m_vC.sub(m_vA) // edge vector AC
	};
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::surfaceToBarycentric(const TVector3<T>& position) const
{
	// Reference: Real-Time Collision Detection, Volume 1, P.47 ~ P.48
	// Computes barycentric coordinates (a, b, c) for a position with 
	// respect to triangle ABC.

	const auto eAP        = position.sub(m_vA);
	const auto [eAB, eAC] = getEdgeVectors();

	const T d00 = eAB.dot(eAB);
	const T d01 = eAB.dot(eAC);
	const T d11 = eAC.dot(eAC);
	const T d20 = eAP.dot(eAB);
	const T d21 = eAP.dot(eAC);
	
	// TODO: check numeric stability

	const T denominator = d00 * d11 - d01 * d01;
	if(denominator == T(0))
	{
		return {0, 0, 0};
	}

	const T rcpDenominator = T(1) / denominator;
	
	const T b = (d11 * d20 - d01 * d21) * rcpDenominator;
	const T c = (d00 * d21 - d01 * d20) * rcpDenominator;
	const T a = T(1) - b - c;

	return {a, b, c};
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::barycentricToSurface(const TVector3<T>& barycentricCoords) const
{
	return TVector3<T>::weightedSum(
		m_vA, barycentricCoords.x,
		m_vB, barycentricCoords.y,
		m_vC, barycentricCoords.z);
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::sampleToBarycentricOsada(const TVector2<T>& sample) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample.x, T(0), T(1));
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample.y, T(0), T(1));

	const T A = std::sqrt(sample.x);
	const T B = sample.y;

	return {
		T(1) - A, 
		A * (T(1) - B), 
		B * A
	};
}

template<typename T>
inline bool TBasicTriangle<T>::isDegenerate() const
{
	const auto [eAB, eAC]  = getEdgeVectors();
	const T    crossFactor = eAB.cross(eAC).lengthSquared();

	// NaN and Inf aware
	return !(crossFactor > T(0)) || std::isinf(crossFactor);
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::getVa() const
{
	return m_vA;
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::getVb() const
{
	return m_vB;
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::getVc() const
{
	return m_vC;
}

}// end namespace ph::math
