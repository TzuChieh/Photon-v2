#pragma once

#include "Common/primitive_type.h"
#include "Math/TArithmeticArray.h"
#include "Math/TVector3.h"
#include "Core/Quantity/spectral_strength_fwd.h"
#include "Core/Quantity/EQuantity.h"

#include <cstddef>
#include <vector>
#include <array>
#include <string>

namespace ph
{

template<typename DerivedType, std::size_t N>
class TAbstractSpectralStrength
{
public:
	static constexpr std::size_t NUM_VALUES = N;

public:
	inline TAbstractSpectralStrength();
	explicit inline TAbstractSpectralStrength(real value);
	explicit inline TAbstractSpectralStrength(const std::array<real, 1>& value);
	explicit inline TAbstractSpectralStrength(const TArithmeticArray<real, 1>& value);
	explicit inline TAbstractSpectralStrength(const std::array<real, N>& values);
	explicit inline TAbstractSpectralStrength(const TArithmeticArray<real, N>& values);
	explicit inline TAbstractSpectralStrength(const TAbstractSpectralStrength& other);
	virtual inline ~TAbstractSpectralStrength() = 0;

public:
	static DerivedType exp(const DerivedType& exponent);

	// Calculates relative luminance with D65 as reference white point.
	//
	inline real calcLuminance(EQuantity valueType = EQuantity::RAW) const;

	inline Vector3R genSrgb(EQuantity valueType = EQuantity::RAW) const;
	inline Vector3R genLinearSrgb(EQuantity valueType = EQuantity::RAW) const;

	inline DerivedType& setSrgb(const Vector3R& srgb, EQuantity valueType = EQuantity::RAW);
	inline DerivedType& setLinearSrgb(const Vector3R& linearSrgb, EQuantity valueType = EQuantity::RAW);
	inline DerivedType& setSampled(const SampledSpectralStrength& sampled, EQuantity valueType = EQuantity::RAW);

	inline DerivedType add(const DerivedType& rhs) const;
	inline DerivedType add(real rhs) const;
	inline DerivedType sub(const DerivedType& rhs) const;
	inline DerivedType sub(real rhs) const;
	inline DerivedType mul(const DerivedType& rhs) const;
	inline DerivedType mul(real rhs) const;
	inline DerivedType div(const DerivedType& rhs) const;
	inline DerivedType div(real rhs) const;

	inline DerivedType& addLocal(const DerivedType& rhs);
	inline DerivedType& addLocal(real rhs);
	inline DerivedType& subLocal(const DerivedType& rhs);
	inline DerivedType& subLocal(real rhs);
	inline DerivedType& mulLocal(const DerivedType& rhs);
	inline DerivedType& mulLocal(real rhs);
	inline DerivedType& divLocal(const DerivedType& rhs);
	inline DerivedType& divLocal(real rhs);

	inline real dot(const DerivedType& rhs) const;
	inline DerivedType pow(integer exponent) const;
	inline DerivedType& sqrtLocal();
	inline DerivedType complement() const;
	inline DerivedType& complementLocal();

	// Inputs must not contain any NaN. 
	// (NaNs are clamped to lower bound)
	//
	inline DerivedType& clampLocal(real lowerBound, real upperBound);

	inline DerivedType& setValues(real value);
	inline DerivedType& setValues(const std::array<real, N>& values);
	inline DerivedType& setValues(const TArithmeticArray<real, N>& values);
	inline DerivedType& setValues(const DerivedType& values);

	inline bool isZero() const;
	inline bool isNonNegative() const;
	inline real sum() const;
	inline real avg() const;
	inline real max() const;

	inline real operator [] (std::size_t index) const;
	inline real& operator [] (std::size_t index);

	inline DerivedType operator + (const DerivedType& rhs) const;
	inline DerivedType operator + (real rhs) const;
	inline DerivedType operator - (const DerivedType& rhs) const;
	inline DerivedType operator - (real rhs) const;
	inline DerivedType operator * (const DerivedType& rhs) const;
	inline DerivedType operator * (real rhs) const;
	inline DerivedType operator / (const DerivedType& rhs) const;
	inline DerivedType operator / (real rhs) const;

	inline std::string toString() const;

protected:
	TArithmeticArray<real, N> m_values;

	inline TAbstractSpectralStrength& operator = (const TAbstractSpectralStrength& rhs);
};

}// end namespace ph