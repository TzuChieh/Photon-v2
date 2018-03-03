#pragma once

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Core/Quantity/ColorSpace.h"
#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"

namespace ph
{

template<typename DerivedType, std::size_t N>
constexpr std::size_t TAbstractSpectralStrength<DerivedType, N>::NUM_VALUES;

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength() :
	TAbstractSpectralStrength(0)
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength(const real value) :
	TAbstractSpectralStrength(TArithmeticArray<real, N>(value))
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength(const TArithmeticArray<real, N>& values) :
	m_values(values)
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength(const TAbstractSpectralStrength& other) :
	m_values(other.m_values)
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectralStrength<DerivedType, N>::~TAbstractSpectralStrength() = default;

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
inline real TAbstractSpectralStrength<DerivedType, N>::calcLuminance() const
{
	return ColorSpace::linear_sRGB_to_luminance(genLinearSrgb());
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

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::add(const TAbstractSpectralStrength& rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.add(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::add(const real rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.add(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::sub(const TAbstractSpectralStrength& rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.sub(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::sub(const real rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.sub(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mul(const TAbstractSpectralStrength& rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.mul(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mul(const real rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.mul(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::div(const TAbstractSpectralStrength& rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.div(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::div(const real rhs) const
	-> TAbstractSpectralStrength<DerivedType, N>
{
	return TAbstractSpectralStrength(m_values.div(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::addLocal(const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.addLocal(rhs.m_values);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::addLocal(const real rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.addLocal(rhs);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::subLocal(const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.subLocal(rhs.m_values);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::subLocal(const real rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.subLocal(rhs);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mulLocal(const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.mulLocal(rhs.m_values);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mulLocal(const real rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.mulLocal(rhs);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::divLocal(const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.divLocal(rhs.m_values);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::divLocal(const real rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.divLocal(rhs);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::set(const real value)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.set(value);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::set(const std::array<real, N>& values)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values.set(values);

	return *this;
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectralStrength<DerivedType, N>::operator [] (const std::size_t index) const
{
	return m_values[index];
}

template<typename DerivedType, std::size_t N>
inline real& TAbstractSpectralStrength<DerivedType, N>::operator [] (const std::size_t index)
{
	return m_values[index];
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::operator + (const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength<DerivedType, N>
{
	m_values 
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::operator * (const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength<DerivedType, N>
{

}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::operator = (const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength<DerivedType, N>&
{
	m_values = rhs.m_values;

	return *this;
}

}// end namespace ph