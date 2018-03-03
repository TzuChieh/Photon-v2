#pragma once

#include "Common/primitive_type.h"
#include "Math/TArithmeticArray.h"
#include "Math/TVector3.h"
#include "Core/Quantity/spectral_strength_fwd.h"

#include <cstddef>
#include <vector>
#include <array>

namespace ph
{

template<typename DerivedType, std::size_t N>
class TAbstractSpectralStrength
{
public:
	static constexpr std::size_t NUM_VALUES = N;

protected:
	inline TAbstractSpectralStrength();
	explicit inline TAbstractSpectralStrength(real value);
	explicit inline TAbstractSpectralStrength(const TArithmeticArray<real, N>& values);
	explicit inline TAbstractSpectralStrength(const TAbstractSpectralStrength& other);
	virtual inline ~TAbstractSpectralStrength() = 0;

public:
	// Calculates relative luminance with D65 as reference white point.
	//
	inline real calcLuminance() const;

	inline Vector3R genSrgb() const;
	inline Vector3R genLinearSrgb() const;
	inline void setSrgb(const Vector3R& srgb);
	inline void setLinearSrgb(const Vector3R& linearSrgb);
	inline void setSampled(const SampledSpectralStrength& sampled);

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

	inline DerivedType pow(integer exponent) const;
	inline DerivedType& sqrtLocal();

	// Inputs must not contain any NaN. 
	// (NaNs are clamped to lower bound)
	//
	inline DerivedType& clampLocal(real lowerBound, real upperBound);

	inline DerivedType& set(real value);
	inline DerivedType& set(const std::array<real, N>& values);

	inline real operator [] (std::size_t index) const;
	inline real& operator [] (std::size_t index);

	inline DerivedType operator + (const DerivedType& rhs);
	inline DerivedType operator * (const DerivedType& rhs);

	inline DerivedType& operator = (const DerivedType& rhs);
	inline TAbstractSpectralStrength& operator = (const TAbstractSpectralStrength& rhs) = delete;

protected:
	TArithmeticArray<real, N> m_values;
};

}// end namespace ph

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.ipp"