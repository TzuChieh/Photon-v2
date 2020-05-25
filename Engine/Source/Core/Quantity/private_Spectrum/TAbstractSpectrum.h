#pragma once

#include "Common/primitive_type.h"
#include "Math/TArithmeticArray.h"
#include "Math/TVector3.h"
#include "Core/Quantity/spectrum_fwd.h"
#include "Core/Quantity/EQuantity.h"

#include <cstddef>
#include <vector>
#include <array>
#include <string>

namespace ph
{

template<typename DerivedType, std::size_t N>
class TAbstractSpectrum
{
public:
	static constexpr std::size_t NUM_VALUES = N;

public:
	explicit TAbstractSpectrum(real value);
	explicit TAbstractSpectrum(const std::array<real, 1>& value);
	explicit TAbstractSpectrum(const math::TArithmeticArray<real, 1>& value);
	explicit TAbstractSpectrum(const std::array<real, N>& values);
	explicit TAbstractSpectrum(const math::TArithmeticArray<real, N>& values);

protected:
	TAbstractSpectrum() = default;
	TAbstractSpectrum(const TAbstractSpectrum& other) = default;
	~TAbstractSpectrum() = default;

public:
	static DerivedType exp(const DerivedType& exponent);

	// Calculates relative luminance with D65 as reference white point.
	//
	real calcLuminance(EQuantity valueType = EQuantity::RAW) const;

	math::Vector3R genSrgb(EQuantity valueType = EQuantity::RAW) const;
	math::Vector3R genLinearSrgb(EQuantity valueType = EQuantity::RAW) const;

	DerivedType& setSrgb(const math::Vector3R& srgb, EQuantity valueType = EQuantity::RAW);
	DerivedType& setLinearSrgb(const math::Vector3R& linearSrgb, EQuantity valueType = EQuantity::RAW);
	DerivedType& setSampled(const SampledSpectrum& sampled, EQuantity valueType = EQuantity::RAW);

	DerivedType add(const DerivedType& rhs) const;
	DerivedType add(real rhs) const;
	DerivedType sub(const DerivedType& rhs) const;
	DerivedType sub(real rhs) const;
	DerivedType mul(const DerivedType& rhs) const;
	DerivedType mul(real rhs) const;
	DerivedType div(const DerivedType& rhs) const;
	DerivedType div(real rhs) const;

	DerivedType& addLocal(const DerivedType& rhs);
	DerivedType& addLocal(real rhs);
	DerivedType& subLocal(const DerivedType& rhs);
	DerivedType& subLocal(real rhs);
	DerivedType& mulLocal(const DerivedType& rhs);
	DerivedType& mulLocal(real rhs);
	DerivedType& divLocal(const DerivedType& rhs);
	DerivedType& divLocal(real rhs);

	real dot(const DerivedType& rhs) const;
	DerivedType pow(integer exponent) const;
	DerivedType& sqrtLocal();
	DerivedType complement() const;
	DerivedType& complementLocal();

	// Inputs must not contain any NaN. 
	// (NaNs are clamped to lower bound)
	//
	DerivedType& clampLocal(real lowerBound, real upperBound);

	DerivedType& setValues(real value);
	DerivedType& setValues(const std::array<real, N>& values);
	DerivedType& setValues(const math::TArithmeticArray<real, N>& values);
	DerivedType& setValues(const DerivedType& values);

	bool isZero() const;
	bool isNonNegative() const;
	bool isFinite() const;
	real sum() const;
	real avg() const;
	real max() const;

	real operator [] (std::size_t index) const;
	real& operator [] (std::size_t index);

	DerivedType operator + (const DerivedType& rhs) const;
	DerivedType operator + (real rhs) const;
	DerivedType operator - (const DerivedType& rhs) const;
	DerivedType operator - (real rhs) const;
	DerivedType operator * (const DerivedType& rhs) const;
	DerivedType operator * (real rhs) const;
	DerivedType operator / (const DerivedType& rhs) const;
	DerivedType operator / (real rhs) const;

	std::string toString() const;

protected:
	math::TArithmeticArray<real, N> m_values;
};

}// end namespace ph
