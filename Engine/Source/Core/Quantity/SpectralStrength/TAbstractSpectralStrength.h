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

public:
	inline TAbstractSpectralStrength();
	explicit inline TAbstractSpectralStrength(real value);
	explicit inline TAbstractSpectralStrength(const TArithmeticArray<real, N>& values);
	explicit inline TAbstractSpectralStrength(const TAbstractSpectralStrength& other);
	virtual inline ~TAbstractSpectralStrength() = 0;

	// Calculates relative luminance with D65 as reference white point.
	//
	inline real calcLuminance() const;

	inline Vector3R genSrgb() const;
	inline Vector3R genLinearSrgb() const;
	inline void setSrgb(const Vector3R& srgb);
	inline void setLinearSrgb(const Vector3R& linearSrgb);
	inline void setSampled(const SampledSpectralStrength& sampled);

	inline TAbstractSpectralStrength add(const TAbstractSpectralStrength& rhs) const;
	inline TAbstractSpectralStrength add(real rhs) const;
	inline TAbstractSpectralStrength sub(const TAbstractSpectralStrength& rhs) const;
	inline TAbstractSpectralStrength sub(real rhs) const;
	inline TAbstractSpectralStrength mul(const TAbstractSpectralStrength& rhs) const;
	inline TAbstractSpectralStrength mul(real rhs) const;
	inline TAbstractSpectralStrength div(const TAbstractSpectralStrength& rhs) const;
	inline TAbstractSpectralStrength div(real rhs) const;

	inline TAbstractSpectralStrength& addLocal(const TAbstractSpectralStrength& rhs);
	inline TAbstractSpectralStrength& addLocal(real rhs);
	inline TAbstractSpectralStrength& subLocal(const TAbstractSpectralStrength& rhs);
	inline TAbstractSpectralStrength& subLocal(real rhs);
	inline TAbstractSpectralStrength& mulLocal(const TAbstractSpectralStrength& rhs);
	inline TAbstractSpectralStrength& mulLocal(real rhs);
	inline TAbstractSpectralStrength& divLocal(const TAbstractSpectralStrength& rhs);
	inline TAbstractSpectralStrength& divLocal(real rhs);

	inline TAbstractSpectralStrength& set(real value);
	inline TAbstractSpectralStrength& set(const std::array<real, N>& values);

	inline real operator [] (std::size_t index) const;
	inline real& operator [] (std::size_t index);

	inline TAbstractSpectralStrength operator + (const TAbstractSpectralStrength& rhs);
	inline TAbstractSpectralStrength operator * (const TAbstractSpectralStrength& rhs);

	inline TAbstractSpectralStrength& operator = (const TAbstractSpectralStrength& rhs);

protected:
	TArithmeticArray<real, N> m_values;
};

}// end namespace ph

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.ipp"