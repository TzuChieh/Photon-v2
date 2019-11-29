#pragma once

#include "Math/constant.h"
#include "Common/assertion.h"

#include <cmath>
#include <array>

namespace ph::math
{

/*! @brief A hemisphere in 3-D space.

The hemisphere is facing up (y-axis) and does not include the disk part 
at the bottom. The origin of the hemisphere is located at the center of the 
full sphere.
*/
template<typename T>
class THemisphere final
{
public:
	static THemisphere makeUnit();

	THemisphere() = default;

	explicit THemisphere(T radius);

	/*! @brief The area of the dome part.
	*/
	T getArea() const;

	/*! @brief Map the 2D sample to a position on the surface of the hemisphere.

	A common mapping method that is based on Archimedes' derivation that
	the horizontal slices of a sphere have equal area. The mapped positions
	are distributed uniformly if the sample is uniform. For a unit hemisphere,
	this method effectively generates normalized directions.
	*/
	TVector3<T> sampleToSurfaceArchimedes(const std::array<T, 2>& sample) const;
	TVector3<T> sampleToSurfaceArchimedes(const std::array<T, 2>& sample, T* out_pdfA) const;

	TVector3<T> sampleToSurfaceCosThetaWeighted(const std::array<T, 2>& sample) const;
	TVector3<T> sampleToSurfaceCosThetaWeighted(const std::array<T, 2>& sample, T* out_pdfA) const;

	TVector3<T> sampleToSurfaceCosLobeWeighted(const std::array<T, 2>& sample, T exponent) const;
	TVector3<T> sampleToSurfaceCosLobeWeighted(const std::array<T, 2>& sample, T exponent, T* out_pdfA) const;

private:
	T m_radius;
};

// In-header Implementations:

template<typename T>
inline THemisphere<T> THemisphere<T>::makeUnit()
{
	return THemisphere(1);
}

template<typename T>
inline THemisphere<T>::THemisphere(const T radius) :
	m_radius(radius)
{
	PH_ASSERT_GE(radius, T(0));
}

template<typename T>
inline T THemisphere<T>::getArea() const
{
	return constant::two_pi<T> * m_radius * m_radius;
}

template<typename T>
inline TVector3<T> THemisphere<T>::sampleToSurfaceArchimedes(const std::array<T, 2>& sample) const
{
	PH_ASSERT_LE(T(0), sample[0]); PH_ASSERT_LE(sample[0], T(1));
	PH_ASSERT_LE(T(0), sample[1]); PH_ASSERT_LE(sample[1], T(1));

	const T phi     = constant::two_pi<T> * sample[0];
	const T yValue  = sample[1];
	const T yRadius = std::sqrt(T(1) - yValue * yValue);

	const auto localUnitPos = TVector3<T>(
		std::sin(phi) * yRadius,
		yValue,
		std::cos(phi) * yRadius);

	return localUnitPos.mul(m_radius);
}

template<typename T>
inline TVector3<T> THemisphere<T>::sampleToSurfaceArchimedes(
	const std::array<T, 2>& sample, T* const out_pdfA) const
{
	// PDF_A is 1/(2*pi*r^2)
	PH_ASSERT(out_pdfA);
	*out_pdfA = T(1) / getArea();

	return sampleToSurfaceArchimedes(sample);
}

template<typename T>
inline TVector3<T> THemisphere<T>::sampleToSurfaceCosThetaWeighted(const std::array<T, 2>& sample) const
{
	PH_ASSERT_LE(T(0), sample[0]); PH_ASSERT_LE(sample[0], T(1));
	PH_ASSERT_LE(T(0), sample[1]); PH_ASSERT_LE(sample[1], T(1));

	const T phi     = constant::two_pi<T> * sample[0];
	const T yValue  = std::sqrt(sample[1]);
	const T yRadius = std::sqrt(T(1) - yValue * yValue);// TODO: y*y is in fact valueB?

	const auto localUnitPos = TVector3<T>(
		std::sin(phi) * yRadius,
		yValue,
		std::cos(phi) * yRadius);

	return localUnitPos.mul(m_radius);
}

template<typename T>
inline TVector3<T> THemisphere<T>::sampleToSurfaceCosThetaWeighted(
	const std::array<T, 2>& sample, T* const out_pdfA) const
{
	const auto localPos = sampleToSurfaceCosThetaWeighted(sample);

	PH_ASSERT_GE(localPos.y, T(0));
	const T cosTheta = localPos.y / m_radius;

	// PDF_A is cos(theta)/(pi*r^2)
	PH_ASSERT(out_pdfA);
	*out_pdfA = cosTheta / (constant::pi<real> * m_radius * m_radius);

	return localPos;
}

template<typename T>
inline TVector3<T> THemisphere<T>::sampleToSurfaceCosLobeWeighted(
	const std::array<T, 2>& sample, const T exponent) const
{
	PH_ASSERT_LE(T(0), sample[0]); PH_ASSERT_LE(sample[0], T(1));
	PH_ASSERT_LE(T(0), sample[1]); PH_ASSERT_LE(sample[1], T(1));

	const T phi      = constant::two_pi<T> * sample[0];
	const T cosTheta = std::pow(sample[1], T(1) / (exponent + T(1)));
	const T sinTheta = std::sqrt(T(1) - cosTheta * cosTheta);

	const auto localUnitPos = TVector3<T>(
		std::sin(phi) * sinTheta,
		cosTheta,
		std::cos(phi) * sinTheta);

	return localUnitPos.mul(m_radius);
}

template<typename T>
inline TVector3<T> THemisphere<T>::sampleToSurfaceCosLobeWeighted(
	const std::array<T, 2>& sample, const T exponent, T* const out_pdfA) const
{
	const auto localPos = sampleToSurfaceCosLobeWeighted(exponent, sample);

	PH_ASSERT_GE(localPos.y, T(0));
	const T cosTheta = localPos.y / m_radius;

	// PDF_A is (exponent+1)/(2*pi)*cos(theta)^n
	PH_ASSERT(out_pdfA);
	*out_pdfA = (exponent + T(1)) * constant::rcp_two_pi<T> * std::pow(cosTheta, exponent);
	*out_pdfA /= m_radius * m_radius;

	return localPos;
}

}// end namespace ph::math
