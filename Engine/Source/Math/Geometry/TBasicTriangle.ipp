#pragma once

#include "Math/Geometry/TBasicTriangle.h"
#include "Common/assertion.h"
#include "Math/TMatrix2.h"

#include <cmath>

namespace ph::math
{

template<typename T>
inline TVector3<T> TBasicTriangle<T>::interpolate(
	const std::array<TVector3<T>, 3>& attributes,
	const TVector3<T>&                barycentricCoords)
{
	return TVector3<T>::weightedSum(
		attributes[0], barycentricCoords.x,
		attributes[1], barycentricCoords.y,
		attributes[2], barycentricCoords.z);
}

template<typename T>
inline bool TBasicTriangle<T>::calcSurfaceParamDerivatives(
	const std::array<TVector3<T>, 3>& attributes,
	const std::array<TVector2<T>, 3>& parameterizations,
	TVector3<T>* const                out_dXdU,
	TVector3<T>* const                out_dXdV)
{
	PH_ASSERT(out_dXdU);
	PH_ASSERT(out_dXdV);

	const auto deltaUv01 = parameterizations[1] - parameterizations[0];
	const auto deltaUv02 = parameterizations[2] - parameterizations[0];

	const TMatrix2<T> A(
		deltaUv01,
		deltaUv02);

	const auto deltaAttr01 = attributes[1] - attributes[0];
	const auto deltaAttr02 = attributes[2] - attributes[0];

	const std::array<std::array<T, 2>, 3> bs = {
		deltaAttr01.x, deltaAttr02.x,
		deltaAttr01.y, deltaAttr02.y,
		deltaAttr01.z, deltaAttr02.z};
	
	std::array<std::array<T, 2>, 3> xs;
	if(A.solve(bs, &xs))
	{
		out_dXdU->x = xs[0][0]; out_dXdV->x = xs[0][1];
		out_dXdU->y = xs[1][0]; out_dXdV->y = xs[1][1];
		out_dXdU->z = xs[2][0]; out_dXdV->z = xs[2][1];

		return true;
	}
	else
	{
		return false;
	}
}

template<typename T>
inline TBasicTriangle<T>::TBasicTriangle(
	TVector3<T> vA,
	TVector3<T> vB,
	TVector3<T> vC) : 

	m_vA(std::move(vA)),
	m_vB(std::move(vB)),
	m_vC(std::move(vC))
{}

template<typename T>
inline TBasicTriangle<T>::TBasicTriangle(std::array<TVector3<T>, 3> vertices) :
	TBasicTriangle(
		std::move(vertices[0]),
		std::move(vertices[1]),
		std::move(vertices[2]))
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
inline TAABB3D<T> TBasicTriangle<T>::getAABB() const
{
	T minX = m_vA.x(), maxX = m_vA.x(),
	  minY = m_vA.y(), maxY = m_vA.y(),
	  minZ = m_vA.z(), maxZ = m_vA.z();

	if     (m_vB.x() > maxX) maxX = m_vB.x();
	else if(m_vB.x() < minX) minX = m_vB.x();
	if     (m_vB.y() > maxY) maxY = m_vB.y();
	else if(m_vB.y() < minY) minY = m_vB.y();
	if     (m_vB.z() > maxZ) maxZ = m_vB.z();
	else if(m_vB.z() < minZ) minZ = m_vB.z();

	if     (m_vC.x() > maxX) maxX = m_vC.x();
	else if(m_vC.x() < minX) minX = m_vC.x();
	if     (m_vC.y() > maxY) maxY = m_vC.y();
	else if(m_vC.y() < minY) minY = m_vC.y();
	if     (m_vC.z() > maxZ) maxZ = m_vC.z();
	else if(m_vC.z() < minZ) minZ = m_vC.z();

	constexpr auto TRIANGLE_EPSILON = T(0.0001);
	
	return math::TAABB3D<T>(
		math::Vector3R(minX - TRIANGLE_EPSILON, minY - TRIANGLE_EPSILON, minZ - TRIANGLE_EPSILON),
		math::Vector3R(maxX + TRIANGLE_EPSILON, maxY + TRIANGLE_EPSILON, maxZ + TRIANGLE_EPSILON));
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
		m_vA, barycentricCoords.x(),
		m_vB, barycentricCoords.y(),
		m_vC, barycentricCoords.z());
}

template<typename T>
inline TVector3<T> TBasicTriangle<T>::sampleToBarycentricOsada(const std::array<T, 2>& sample) const
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[0], T(0), T(1));
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample[1], T(0), T(1));

	const T A = std::sqrt(sample[0]);
	const T B = sample[1];

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
