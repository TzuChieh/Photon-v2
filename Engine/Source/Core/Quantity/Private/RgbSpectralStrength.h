#pragma once

#include "Core/Quantity/Private/TAbstractSpectralStrength.h"

namespace ph
{

class RgbSpectralStrength final : public TAbstractSpectralStrength<RgbSpectralStrength, 3>
{
public:
	using TAbstractSpectralStrength<RgbSpectralStrength, 3>::TAbstractSpectralStrength;

	RgbSpectralStrength();
	RgbSpectralStrength(const RgbSpectralStrength& other);
	RgbSpectralStrength(const TArithmeticArray<real, 3>& other);
	virtual ~RgbSpectralStrength() override;

	Vector3R internal_genRgb() const;
	void internal_setRgb(const Vector3R& rgb);

	using TAbstractSpectralStrength<RgbSpectralStrength, 3>::operator = ;
};

}// end namespace ph