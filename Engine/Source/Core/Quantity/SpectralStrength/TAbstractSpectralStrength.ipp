#pragma once

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Core/Quantity/ColorSpace.h"
#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"

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
inline Vector3R TAbstractSpectralStrength<DerivedType, N>::genSrgb() const
{
	return ColorSpace::linear_sRGB_to_sRGB(genLinearSrgb());
}

template<typename DerivedType, std::size_t N>
inline Vector3R TAbstractSpectralStrength<DerivedType, N>::genLinearSrgb() const
{
	return static_cast<const DerivedType&>(*this).impl_genLinearSrgb();
}

template<typename DerivedType, std::size_t N>
inline void TAbstractSpectralStrength<DerivedType, N>::setSrgb(const Vector3R& srgb)
{
	setLinearSrgb(ColorSpace::sRGB_to_linear_sRGB(srgb));
}

template<typename DerivedType, std::size_t N>
inline void TAbstractSpectralStrength<DerivedType, N>::setLinearSrgb(const Vector3R& linearSrgb)
{
	static_cast<DerivedType&>(*this).impl_setLinearSrgb(linearSrgb);
}

template<typename DerivedType, std::size_t N>
inline void TAbstractSpectralStrength<DerivedType, N>::setSampled(
	const SampledSpectralStrength& sampled)
{
	static_cast<DerivedType&>(*this).impl_setSampled(sampled);
}

}// end namespace ph