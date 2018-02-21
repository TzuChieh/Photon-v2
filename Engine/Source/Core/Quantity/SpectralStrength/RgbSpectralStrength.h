#pragma once

#include "Core/Quantity/SpectralStrength/TAbstractSpectralStrength.h"

namespace ph
{

class RgbSpectralStrength final : public TAbstractSpectralStrength<RgbSpectralStrength, 3>
{
public:
	using TAbstractSpectralStrength<RgbSpectralStrength, 3>::TAbstractSpectralStrength;
	RgbSpectralStrength();
	RgbSpectralStrength(const RgbSpectralStrength& other);
	RgbSpectralStrength(const TArithmeticArray<real, 3>& other);
	virtual inline ~RgbSpectralStrength() override = default;

	using TAbstractSpectralStrength<RgbSpectralStrength, 3>::operator = ;

	Vector3R impl_genRgb() const;
	void impl_setRgb(const Vector3R& rgb);
	void impl_setSampled(const std::vector<SpectralSample>& samples);
};

}// end namespace ph