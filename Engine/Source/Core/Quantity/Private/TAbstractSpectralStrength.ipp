#pragma once

#include "Core/Quantity/Private/TAbstractSpectralStrength.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"

namespace ph
{

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength() :
	TArithmeticArray<real, N>(0)
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength(const TAbstractSpectralStrength& other) :
	TArithmeticArray<real, N>(other)
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength(const TArithmeticArray<real, N>& other) :
	TArithmeticArray<real, N>(other)
{}

template<typename DerivedType, std::size_t N>
inline Vector3R TAbstractSpectralStrength<DerivedType, N>::genRgb() const
{
	return static_cast<const DerivedType&>(*this).impl_genRgb();
}

template<typename DerivedType, std::size_t N>
inline void TAbstractSpectralStrength<DerivedType, N>::setRgb(const Vector3R& rgb)
{
	static_cast<DerivedType&>(*this).impl_setRgb(rgb);
}

template<typename DerivedType, std::size_t N>
inline void TAbstractSpectralStrength<DerivedType, N>::setSampled(
	const std::vector<SpectralSample>& samples)
{
	static_cast<DerivedType&>(*this).impl_setSampled(samples);
}

}// end namespace ph