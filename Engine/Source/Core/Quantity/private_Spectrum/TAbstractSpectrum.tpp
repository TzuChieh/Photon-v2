#pragma once

#include "Core/Quantity/private_Spectrum/TAbstractSpectrum.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Core/Quantity/ColorSpace.h"

#include <cmath>

namespace ph
{

template<typename DerivedType, std::size_t N>
constexpr std::size_t TAbstractSpectrum<DerivedType, N>::NUM_VALUES;

template<typename DerivedType, std::size_t N>
inline TAbstractSpectrum<DerivedType, N>::TAbstractSpectrum(const real value) :
	TAbstractSpectrum(math::TArithmeticArray<real, N>(value))
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectrum<DerivedType, N>::TAbstractSpectrum(const std::array<real, 1>& value) : 
	TAbstractSpectrum(value[0])
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectrum<DerivedType, N>::TAbstractSpectrum(const math::TArithmeticArray<real, 1>& value) :
	TAbstractSpectrum(value[0])
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectrum<DerivedType, N>::TAbstractSpectrum(const std::array<real, N>& values) :
	TAbstractSpectrum(math::TArithmeticArray<real, N>(values))
{}

template<typename DerivedType, std::size_t N>
inline TAbstractSpectrum<DerivedType, N>::TAbstractSpectrum(const math::TArithmeticArray<real, N>& values) :
	m_values(values)
{}

template<typename DerivedType, std::size_t N>
inline math::Vector3R TAbstractSpectrum<DerivedType, N>::genSrgb(const EQuantity valueType) const
{
	return ColorSpace::linear_sRGB_to_sRGB(genLinearSrgb(valueType));
}

template<typename DerivedType, std::size_t N>
inline math::Vector3R TAbstractSpectrum<DerivedType, N>::genLinearSrgb(const EQuantity valueType) const
{
	return static_cast<const DerivedType&>(*this).impl_genLinearSrgb(valueType);
}

template<typename DerivedType, std::size_t N>
inline DerivedType TAbstractSpectrum<DerivedType, N>::exp(const DerivedType& exponent)
{
	DerivedType result;
	for(std::size_t i = 0; i < N; ++i)
	{
		result[i] = std::exp(exponent[i]);
	}
	return result;
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectrum<DerivedType, N>::calcLuminance(const EQuantity valueType) const
{
	const math::Vector3R& linearSrgb = genLinearSrgb(valueType);

	switch(valueType)
	{
	case EQuantity::EMR:
		return ColorSpace::linear_sRGB_D65_to_luminance(linearSrgb);

	case EQuantity::ECF:
		return ColorSpace::linear_sRGB_E_to_luminance(linearSrgb);

	default:
		return ColorSpace::linear_sRGB_E_to_luminance(linearSrgb);
	}
}

template<typename DerivedType, std::size_t N>
inline DerivedType& TAbstractSpectrum<DerivedType, N>::setSrgb(const math::Vector3R& srgb, const EQuantity valueType)
{
	setLinearSrgb(ColorSpace::sRGB_to_linear_sRGB(srgb), valueType);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline DerivedType& TAbstractSpectrum<DerivedType, N>::setLinearSrgb(const math::Vector3R& linearSrgb, const EQuantity valueType)
{
	static_cast<DerivedType&>(*this).impl_setLinearSrgb(linearSrgb, valueType);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline DerivedType& TAbstractSpectrum<DerivedType, N>::setSampled(
	const SampledSpectrum& sampled, const EQuantity valueType)
{
	static_cast<DerivedType&>(*this).impl_setSampled(sampled, valueType);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::add(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.add(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::add(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.add(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::sub(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.sub(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::sub(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.sub(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::mul(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.mul(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::mul(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.mul(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::div(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.div(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::div(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.div(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::addLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.addLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::addLocal(const real rhs)
	-> DerivedType&
{
	m_values.addLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::subLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.subLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::subLocal(const real rhs)
	-> DerivedType&
{
	m_values.subLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::mulLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.mulLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::mulLocal(const real rhs)
	-> DerivedType&
{
	m_values.mulLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::divLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.divLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::divLocal(const real rhs)
	-> DerivedType&
{
	m_values.divLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::complementLocal()
-> DerivedType&
{
	m_values.complementLocal();

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectrum<DerivedType, N>::dot(const DerivedType& rhs) const
{
	return m_values.dot(rhs.m_values);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::pow(const integer exponent) const
	-> DerivedType
{
	return DerivedType(m_values.pow(exponent));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::sqrtLocal()
	-> DerivedType&
{
	m_values.sqrtLocal();

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::complement() const
	-> DerivedType
{
	return DerivedType(m_values.complement());
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::clampLocal(const real lowerBound, const real upperBound)
	-> DerivedType&
{
	m_values.clampLocal(lowerBound, upperBound);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::setValues(const real value)
	-> DerivedType&
{
	m_values.set(value);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::setValues(const std::array<real, N>& values)
	-> DerivedType&
{
	m_values.set(values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::setValues(const math::TArithmeticArray<real, N>& values)
	-> DerivedType&
{
	m_values = values;

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::setValues(const DerivedType& values)
	-> DerivedType&
{
	return setValues(values.m_values);
}

template<typename DerivedType, std::size_t N>
inline bool TAbstractSpectrum<DerivedType, N>::isZero() const
{
	return m_values.isZero();
}

template<typename DerivedType, std::size_t N>
inline bool TAbstractSpectrum<DerivedType, N>::isNonNegative() const
{
	return m_values.isNonNegative();
}

template<typename DerivedType, std::size_t N>
inline bool TAbstractSpectrum<DerivedType, N>::isFinite() const
{
	return m_values.isFinite();
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectrum<DerivedType, N>::sum() const
{
	return m_values.sum();
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectrum<DerivedType, N>::avg() const
{
	return m_values.avg();
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectrum<DerivedType, N>::max() const
{
	return m_values.max();
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::abs() const
	-> DerivedType
{
	return DerivedType(m_values.abs());
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectrum<DerivedType, N>::operator [] (const std::size_t index) const
{
	return m_values[index];
}

template<typename DerivedType, std::size_t N>
inline real& TAbstractSpectrum<DerivedType, N>::operator [] (const std::size_t index)
{
	return m_values[index];
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator + (const DerivedType& rhs) const
	-> DerivedType
{
	return this->add(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator + (const real rhs) const
	-> DerivedType
{
	return this->add(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator - (const DerivedType& rhs) const
	-> DerivedType
{
	return this->sub(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator - (const real rhs) const
	-> DerivedType
{
	return this->sub(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator * (const DerivedType& rhs) const
	-> DerivedType
{
	return this->mul(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator * (const real rhs) const
	-> DerivedType
{
	return this->mul(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator / (const DerivedType& rhs) const
	-> DerivedType
{
	return this->div(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::operator / (const real rhs) const
	-> DerivedType
{
	return this->div(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectrum<DerivedType, N>::toString() const
	-> std::string
{
	return m_values.toString();
}

}// end namespace ph
