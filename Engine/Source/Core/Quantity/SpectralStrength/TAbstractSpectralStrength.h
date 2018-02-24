#pragma once

#include "Common/primitive_type.h"
#include "Math/TArithmeticArray.h"
#include "Math/TVector3.h"
#include "Core/Quantity/spectral_strength_fwd.h"

#include <cstddef>
#include <vector>

namespace ph
{

template<typename DerivedType, std::size_t N>
class TAbstractSpectralStrength : public TArithmeticArray<real, N>
{
public:
	using TArithmeticArray<real, N>::TArithmeticArray;
	inline TAbstractSpectralStrength();
	inline TAbstractSpectralStrength(const TAbstractSpectralStrength& other);
	inline TAbstractSpectralStrength(const TArithmeticArray<real, N>& other);
	virtual inline ~TAbstractSpectralStrength() override = default;

	inline Vector3R genSrgb() const;
	inline Vector3R genLinearSrgb() const;
	inline void setSrgb(const Vector3R& srgb);
	inline void setLinearSrgb(const Vector3R& linearSrgb);
	inline void setSampled(const SampledSpectralStrength& sampled);

	using TArithmeticArray<real, N>::operator = ;
};

}// end namespace ph

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.ipp"