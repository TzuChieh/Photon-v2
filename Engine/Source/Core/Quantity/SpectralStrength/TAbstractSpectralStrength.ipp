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
inline TAbstractSpectralStrength<DerivedType, N>::TAbstractSpectralStrength(const std::array<real, N>& values) :
	TAbstractSpectralStrength(TArithmeticArray<real, N>(values))
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
inline Vector3R TAbstractSpectralStrength<DerivedType, N>::genSrgb(const EQuantity valueType) const
{
	return ColorSpace::linear_sRGB_to_sRGB(genLinearSrgb(valueType));
}

template<typename DerivedType, std::size_t N>
inline Vector3R TAbstractSpectralStrength<DerivedType, N>::genLinearSrgb(const EQuantity valueType) const
{
	return static_cast<const DerivedType&>(*this).impl_genLinearSrgb(valueType);
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectralStrength<DerivedType, N>::calcLuminance(const EQuantity valueType) const
{
	const Vector3R& linearSrgb = genLinearSrgb(valueType);

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
inline void TAbstractSpectralStrength<DerivedType, N>::setSrgb(const Vector3R& srgb, const EQuantity valueType)
{
	setLinearSrgb(ColorSpace::sRGB_to_linear_sRGB(srgb), valueType);
}

template<typename DerivedType, std::size_t N>
inline void TAbstractSpectralStrength<DerivedType, N>::setLinearSrgb(const Vector3R& linearSrgb, const EQuantity valueType)
{
	static_cast<DerivedType&>(*this).impl_setLinearSrgb(linearSrgb, valueType);
}

template<typename DerivedType, std::size_t N>
inline void TAbstractSpectralStrength<DerivedType, N>::setSampled(
	const SampledSpectralStrength& sampled, const EQuantity valueType)
{
	static_cast<DerivedType&>(*this).impl_setSampled(sampled, valueType);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::add(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.add(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::add(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.add(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::sub(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.sub(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::sub(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.sub(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mul(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.mul(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mul(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.mul(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::div(const DerivedType& rhs) const
	-> DerivedType
{
	return DerivedType(m_values.div(rhs.m_values));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::div(const real rhs) const
	-> DerivedType
{
	return DerivedType(m_values.div(rhs));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::addLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.addLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::addLocal(const real rhs)
	-> DerivedType&
{
	m_values.addLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::subLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.subLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::subLocal(const real rhs)
	-> DerivedType&
{
	m_values.subLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mulLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.mulLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::mulLocal(const real rhs)
	-> DerivedType&
{
	m_values.mulLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::divLocal(const DerivedType& rhs)
	-> DerivedType&
{
	m_values.divLocal(rhs.m_values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::divLocal(const real rhs)
	-> DerivedType&
{
	m_values.divLocal(rhs);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::complementLocal()
-> DerivedType&
{
	m_values.complementLocal();

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectralStrength<DerivedType, N>::dot(const DerivedType& rhs) const
{
	return m_values.dot(rhs.m_values);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::pow(const integer exponent) const
	-> DerivedType
{
	return DerivedType(m_values.pow(exponent));
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::sqrtLocal()
	-> DerivedType&
{
	m_values.sqrtLocal();

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::complement() const
	-> DerivedType
{
	return DerivedType(m_values.complement());
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::clampLocal(const real lowerBound, const real upperBound)
	-> DerivedType&
{
	m_values.clampLocal(lowerBound, upperBound);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::setValues(const real value)
	-> DerivedType&
{
	m_values.set(value);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::setValues(const std::array<real, N>& values)
	-> DerivedType&
{
	m_values.set(values);

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::setValues(const TArithmeticArray<real, N>& values)
	-> DerivedType&
{
	m_values = values;

	return static_cast<DerivedType&>(*this);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::setValues(const DerivedType& values)
	-> DerivedType&
{
	return setValues(values.m_values);
}

template<typename DerivedType, std::size_t N>
inline bool TAbstractSpectralStrength<DerivedType, N>::isZero() const
{
	return m_values.isZero();
}

template<typename DerivedType, std::size_t N>
inline bool TAbstractSpectralStrength<DerivedType, N>::isNonNegative() const
{
	return m_values.isNonNegative();
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectralStrength<DerivedType, N>::sum() const
{
	return m_values.sum();
}

template<typename DerivedType, std::size_t N>
inline real TAbstractSpectralStrength<DerivedType, N>::avg() const
{
	return m_values.avg();
}


template<typename DerivedType, std::size_t N>
inline real TAbstractSpectralStrength<DerivedType, N>::max() const
{
	return m_values.max();
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
inline auto TAbstractSpectralStrength<DerivedType, N>::operator + (const DerivedType& rhs) const
	-> DerivedType
{
	return this->add(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::operator * (const DerivedType& rhs) const
	-> DerivedType
{
	return this->mul(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::operator + (const real rhs) const
	-> DerivedType
{
	return this->add(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::operator * (const real rhs) const
	-> DerivedType
{
	return this->mul(rhs);
}

template<typename DerivedType, std::size_t N>
inline auto TAbstractSpectralStrength<DerivedType, N>::operator = (const TAbstractSpectralStrength& rhs)
	-> TAbstractSpectralStrength&
{
	m_values = rhs.m_values;

	return *this;
}

}// end namespace ph