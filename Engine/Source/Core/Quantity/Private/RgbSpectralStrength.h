#pragma once

#include "Core/Quantity/Private/TAbstractSpectralStrength.h"

namespace ph
{

class RgbSpectralStrength final : public TAbstractSpectralStrength<RgbSpectralStrength, 3>
{
public:
	virtual ~RgbSpectralStrength() override;
};

}// end namespace ph