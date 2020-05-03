#pragma once

#include "Core/Quantity/private_Spectrum/TAbstractSpectrum.h"
#include "Math/TVector3.h"

namespace ph
{

class LinearSrgbSpectrum : 
	public TAbstractSpectrum<LinearSrgbSpectrum, 3>
{
public:
	using Parent = TAbstractSpectrum<LinearSrgbSpectrum, 3>;

	inline LinearSrgbSpectrum() = default;
	inline LinearSrgbSpectrum(const LinearSrgbSpectrum& other) = default;
	using Parent::Parent;

	math::Vector3R impl_genLinearSrgb(EQuantity valueType) const;
	void impl_setLinearSrgb(const math::Vector3R& rgb, EQuantity valueType);
	void impl_setSampled(const SampledSpectrum& sampled, EQuantity valueType);
};

}// end namespace ph
